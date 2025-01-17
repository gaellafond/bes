// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of the BES, A C++ implementation of the OPeNDAP Data
// Access Protocol.

// Copyright (c) 2018 OPeNDAP, Inc.
// Author: Nathan Potter <ndp@opendap.org>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

#include "config.h"

#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h> // for wait
#include <sstream>      // std::stringstream

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <vector>
#include <string>
#include <memory>

#include <BESInternalError.h>
#include <BESInternalFatalError.h>

#include "TempFile.h"
#include "BESLog.h"
#include "BESUtil.h"
#include "BESDebug.h"

using namespace std;

#define MODULE "dap"
#define prolog string("TempFile::").append(__func__).append("() - ")

#define STRICT 0

namespace bes {

std::once_flag TempFile::d_init_once;
std::unique_ptr< std::map<std::string, int> > TempFile::open_files;
struct sigaction TempFile::cached_sigpipe_handler;


/**
 * We need to make sure that all of the open temporary files get cleaned up if
 * bad things happen. So far, SIGPIPE is the only bad thing we know about
 * at least with respect to the TempFile class.
 */
void TempFile::sigpipe_handler(int sig)
{
    try {
        if (sig == SIGPIPE) {
            for (const auto &mpair: *open_files) {
                if (unlink((mpair.first).c_str()) == -1) {
                    stringstream msg;
                    msg << "Error unlinking temporary file: '" << mpair.first << "'";
                    msg << " errno: " << errno << " message: " << strerror(errno) << endl;
                    ERROR_LOG(msg.str());
                }
            }
            // Files cleaned up? Sweet! Time to bail...
            // Remove this SIGPIPE handler
            sigaction(SIGPIPE, &cached_sigpipe_handler, nullptr);
            // Re-raise SIGPIPE
            raise(SIGPIPE);
        }
    }
    catch (BESError &e) {
        cerr << "Encountered BESError. Message: " << e.get_verbose_message();
        cerr << " (location: " << __FILE__ << " at line: " << __LINE__ << ")" <<  endl;
    }
    catch (...) {
        cerr << "Encountered unknown error in " << __FILE__ << " at line: " << __LINE__ << endl;
    }
}


/**
 * @brief Attempts to create the directory identified by dir_name, throws an exception if it fails.
 * @param dir_name
 */
void TempFile::mk_temp_dir(const std::string &dir_name) {

    mode_t mode = S_IRWXU | S_IRWXG;
    stringstream ss;
    ss << prolog << "mode: " <<  std::oct << mode << endl;
    BESDEBUG(MODULE, ss.str());

    if(mkdir(dir_name.c_str(), mode)){
        if(errno != EEXIST){
            stringstream msg;
            msg << prolog  << "ERROR - Failed to create temp directory: " << dir_name;
            msg << " errno: " << errno << " reason: " << strerror(errno);
            throw BESInternalFatalError(msg.str(),__FILE__,__LINE__);
        }
        else {
            BESDEBUG(MODULE,prolog << "The temp directory: " << dir_name << " exists." << endl);
#if STRICT
            uid_t uid = getuid();
            gid_t gid = getgid();
            BESDEBUG(MODULE,prolog << "Assuming ownership of " << dir_name << " (uid: " << uid << " gid: "<< gid << ")" << endl);
            if(chown(dir_name.c_str(),uid,gid)){
                stringstream msg;
                msg << prolog  << "ERROR - Failed to assume ownership (uid: "<< uid;
                msg << " gid: " << gid << ") of: " << dir_name;
                msg << " errno: " << errno << " reason: " << strerror(errno);
                throw BESInternalFatalError(msg.str(),__FILE__,__LINE__);
            }
            BESDEBUG(MODULE,prolog << "Changing permissions to mode: " << std::oct << mode << endl);
            if(chmod(dir_name.c_str(),mode)){
                stringstream msg;
                msg << prolog  << "ERROR - Failed to change permissions (mode: " << std::oct << mode;
                msg << ") for: " << dir_name;
                msg << " errno: " << errno << " reason: " << strerror(errno);
                throw BESInternalFatalError(msg.str(),__FILE__,__LINE__);
            }
#endif
        }
    }
    else {
        BESDEBUG(MODULE,prolog << "The temp directory: " << dir_name << " was created." << endl);
    }
}

/**
 * @brief Initialize static class members, should only be called once using std::call_once()
 */
void TempFile::init() {
    open_files.reset(new std::map<string, int>());
}

/**
 *
 * @param keep_temps Keep the temporary files.
 */
TempFile::TempFile(bool keep_temps): d_keep_temps(keep_temps) {
    std::call_once(d_init_once,TempFile::init);
}


/**
 * @brief Create a new temporary file
 *
 * Get a new temporary file using the given directory and temporary file prefix.
 * If the directory does not exist it will be created.
 *
 * @param dir_name The name of the directory in which the temporary file
 * will be created.
 * @param temp_file_prefix A prefix to be used for the temporary file.
 * @return The name of the temporary file.
 */
string TempFile::create(const std::string &dir_name, const std::string &temp_file_prefix)
{
    std::lock_guard<std::recursive_mutex> lock_me(d_tf_lock_mutex);

    BESDEBUG(MODULE, prolog << "dir_name: " << dir_name << endl);
    mk_temp_dir(dir_name);

    BESDEBUG(MODULE, prolog << "temp_file_prefix: " << temp_file_prefix << endl);
    string tmplt("XXXXXX");
    if(!BESUtil::endsWith(temp_file_prefix,"_")){
        tmplt = "_" + tmplt;
    }
    string file_template = temp_file_prefix + tmplt;
    BESDEBUG(MODULE, prolog << "file_template: " << file_template << endl);

    string target_file = BESUtil::pathConcat(dir_name,file_template);
    BESDEBUG(MODULE, prolog << "target_file: " << target_file << endl);

    char tmp_name[target_file.length() + 1];
    std::string::size_type len = target_file.copy(tmp_name, target_file.length());
    tmp_name[len] = '\0';

    // cover the case where older versions of mkstemp() create the file using
    // a mode of 666.
    mode_t original_mode = umask(077);
    d_fd = mkstemp(tmp_name);
    umask(original_mode);

    if (d_fd == -1) {
        stringstream msg;
        msg << "Failed to open the temporary file using mkstemp()";
        msg << " errno: " << errno << " message: " << strerror(errno);
        msg << " FileTemplate: " + target_file;
        throw BESInternalError(msg.str(), __FILE__, __LINE__);
    }
    d_fname.assign(tmp_name);

    // Check to see if there are already active TempFile things,
    // we can tell because if open_files->size() is zero then this
    // is the first, and we need to register SIGPIPE handler.
    if (open_files->empty()) {
        struct sigaction act;
        sigemptyset(&act.sa_mask);
        sigaddset(&act.sa_mask, SIGPIPE);
        act.sa_flags = 0;

        act.sa_handler = bes::TempFile::sigpipe_handler;

        if (sigaction(SIGPIPE, &act, &cached_sigpipe_handler)) {
            stringstream msg;
            msg << "Could not register a handler to catch SIGPIPE.";
            msg << " errno: " << errno << " message: " << strerror(errno);
            throw BESInternalFatalError(msg.str(), __FILE__, __LINE__);
        }
    }
    open_files->insert(std::pair<string, int>(d_fname, d_fd));

    return d_fname;
}

/**
 * @brief Free the temporary file
 *
 * Close the open descriptor and delete (unlink) the file name.
 */
TempFile::~TempFile()
{
    try {
        if(d_fd != -1 && close(d_fd) == -1) {
            stringstream msg;
            msg << "Error closing temporary file: '" << d_fname ;
            msg << " errno: " << errno << " message: " << strerror(errno) << endl;
            ERROR_LOG(msg.str());
        }
        if(!d_fname.empty()) {
            std::lock_guard<std::recursive_mutex> lock_me(d_tf_lock_mutex);
            if (!d_keep_temps) {
                if (unlink(d_fname.c_str()) == -1) {
                    stringstream msg;
                    msg << "Error unlinking temporary file: '" << d_fname ;
                    msg << " errno: " << errno << " message: " << strerror(errno) << endl;
                    ERROR_LOG(msg.str());
                }
            }
            open_files->erase(d_fname);
            if (open_files->empty()) {
                // No more files means we can unload the SIGPIPE handler
                // If more files are created at a later time then it will get reloaded.
                if (sigaction(SIGPIPE, &cached_sigpipe_handler, nullptr)) {
                    stringstream msg;
                    msg << "Could not de-register the SIGPIPE handler function cached_sigpipe_handler(). ";
                    msg << " errno: " << errno << " message: " << strerror(errno);
                    ERROR_LOG(msg.str());
                }
            }
        }
    }
    catch (BESError const &e) {
        cerr << "Encountered BESError will closing " << d_fname << " Message: " << e.get_verbose_message();
        cerr << " (location: " << __FILE__ << " at line: " << __LINE__ << ")" <<  endl;
    }
    catch (...) {
        cerr << "Encountered unknown error while closing " << d_fname;
        cerr << "  " << __FILE__ << " at line: " << __LINE__ << endl;
    }
}

} // namespace bes

