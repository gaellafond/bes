import argparse as ap
import glob
import os
import shutil
import subprocess
import sys


def cp_testing_files():
    dest_dir =os.getcwd()
    for file in glob.glob('test_reduce_mdf_data/*.h5'):
        shutil.copy(file, dest_dir)
    for file in glob.glob('./*missing.h5'):
        os.remove(file)
    for file in glob.glob('test_reduce_mdf_data/*.h5.dmrpp'):
        shutil.copy(file, dest_dir)
    for file in glob.glob('./*missing.h5.dmrpp'):
        os.remove(file)

def rm_testing_files(choice):
    if(choice!=1 and choice!=2):
        if(choice == 0):
            print("Testing step 4. Remove all the intermediate testing files.\n")
        dest_dir =os.getcwd()
        for file in glob.glob('*.h5'):
            os.remove(file)
        for file in glob.glob('*.h5.dmrpp'):
            os.remove(file)
        os.remove('mh5_sha256')

def check_testing_files(fname,choice):
    cur_dir=os.getcwd()
    dmrpp_file = open(fname,'r')
    dmrpp_text = dmrpp_file.read()
    dmrpp_file.close()
    new_dir="testdir"
    dmrpp_text = dmrpp_text.replace(cur_dir,new_dir)
    #print(dmrpp_text)
    dmrpp_file = open(fname,'w')
    dmrpp_file.write(dmrpp_text)
    dmrpp_file.close()
    fname_patch = "test_reduce_mdf_data/"+fname.rsplit(".dmrpp")[0]+".patched.dmrpp"
    ret=subprocess.run(["diff",fname,fname_patch])
    if ret.returncode!=0:
        print(fname,"is not patched as expected, stop.")
        rm_testing_files(choice)
        sys.exit(1)

parser = ap.ArgumentParser(description='Testing the program to patch and reduce the number of missing hdf5 files.')
parser.add_argument("-v","--verbosity",type=int, choices=[0,1,2],
                     help='Value=0, more output messages. '
                          'Value=1, also keep intermediate files. '
                          'Value=2, also output verbose messages generated by the patch python program..')
args = parser.parse_args()

patch_all_verbose_choice = -1
if(args.verbosity is not None): 
    patch_all_verbose_choice = args.verbosity

# Copy the testing data
# To avoid the interactive copy. Here I obtain the system cp.
# May also use shutil copy files if having more tests
if(patch_all_verbose_choice >=0): 
    print("\nTesting step 1. Copying all the files necessary for testing to the current directory.")
cp_testing_files();

if(patch_all_verbose_choice >=0): 
    print("\nTesting step 2. Patching all the necessary files.")

if(patch_all_verbose_choice == 2): 
    print("------ Coming to the patch program -------")
    ret=subprocess.run(["python","./patch_all_no_dup_mh5.py","-v",str(patch_all_verbose_choice),"-i","mh5_sha256"])
else:
    ret=subprocess.run(["python","./patch_all_no_dup_mh5.py","-i","mh5_sha256"])
if ret.returncode!=0:
    print("The testing program doesn't run successfully, stop.")
    rm_testing_files(patch_all_verbose_choice)
    sys.exit(1)

# Now check the output
# Case 1: d_int.h5.dmrpp doesn't have missing variable values. 
#         It should not be changed.

if(patch_all_verbose_choice >=0): 
    print("Testing step 3. Check the testing results: \n")
ret=subprocess.run(["diff","d_int.h5.dmrpp","test_reduce_mdf_data/d_int.h5.dmrpp"])
if ret.returncode!=0:
    print("d_int.h5.dmrpp should not be changed, stop.")
    rm_testing_files(patch_all_verbose_choice)
    sys.exit(1)

if(patch_all_verbose_choice >=0): 
    print("  T3.1: For dmrpp files that don't have missing variable values: the test gets passed.")

#Case 2: 
#grid_1_2d.h5.dmrpp and grid_1_2d_convention.h5.dmrpp share the same missing HDF5 file. 
#t_cf_1dllz and t_cf_2dllz also share the same missing HDF5 file.
#The missing variable value location is provided.
#To test, we need to figure out 
# (1) If the location is correct.
# (2) If the chunk info is the same as the expected output.
# (3) If the missing h5 name is the same as the expected output.
check_testing_files("grid_1_2d.h5.dmrpp",patch_all_verbose_choice)
check_testing_files("grid_1_2d_convention.h5.dmrpp",patch_all_verbose_choice)
check_testing_files("t_cf_1dllz.h5.dmrpp",patch_all_verbose_choice)
check_testing_files("t_cf_2dllz.h5.dmrpp",patch_all_verbose_choice)

#Case 3: we also need to compare the file that stores the sha256
ret=subprocess.run(["diff","mh5_sha256","test_reduce_mdf_data/mh5_sha256"])
if ret.returncode!=0:
    print("The recording file is wrong, stop.")
    rm_testing_files(patch_all_verbose_choice)
    sys.exit(1)

if(patch_all_verbose_choice >=0): 
    print("  T3.2: For dmrpp files that have missing variable values: the test gets passed. \n")

rm_testing_files(patch_all_verbose_choice)
print("Reduce the number of missing data files: Tests succeed. All tests get passed.")

