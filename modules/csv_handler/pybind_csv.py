import pybindCSV
import pybindDAS

d = pybindDAS.DAS()
c = pybindCSV.CSVHandler("csv")

c.csv_read_attributes(d, "data/temperature.csv")

