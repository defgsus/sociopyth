# encoding=utf-8

"""
More or less example code of reading the company xml file
"""

from kek_tools import kek_read_company_xml, Company, Share



########################################################################


companies = kek_read_company_xml("./kek_owner.xml")


# print companies		
for c in companies.values():
	print "\ncompany: " + c.name
	for m in c.titles:
		print ' media: "' + m + '"'
	for s in c.shares.values():
		print ' share: ' + str(s.percent) + '% "' + s.name + '"'

