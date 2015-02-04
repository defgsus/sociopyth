# encoding=utf-8

"""
More or less example code of reading the company xml file
"""

from kek_tools import kek_read_company_xml, Company, Share



def kek_export_neato(companies, filename):
	"""Exports the owner relations to neato - not really useful."""
	f = open(filename, "wt")
	
	c_id = {}
	k = 0
	# company nodes
	f.write("graph {\n")
	for c in companies.values():
		# node id for graphviz
		k = k + 1
		cid = "n" + str(k)
		c_id[c.name] = cid
		f.write((cid + ' [label="' + c.name.replace('"', "'") + '"];\n').encode("UTF-8"))
		
	# edges
	for c in companies.values():
		cid = c_id[c.name]
		for s in c.shares.values():
			sid = c_id[s.name]
			f.write((cid + " -- " + sid + ' [label="' + str(s.percent) + '%"];\n').encode("UTF-8"))
			# neato crashes with this..
			#f.write((cid + " -- " + sid + ' [len=' + str(100. / (1.+s.percent)) + "];\n").encode("UTF-8"))
	
	f.write("}")
	f.close()




########################################################################


companies = kek_read_company_xml("./kek_owner.xml")

#kek_export_neato(companies, "kek_owner.dot"); quit()

# print companies		
for c in companies.values():
	print "\ncompany: " + c.name
	for m in c.titles:
		print ' media: "' + m + '"'
	for s in c.shares.values():
		print ' share: ' + str(s.percent) + '% "' + s.name + '"'

