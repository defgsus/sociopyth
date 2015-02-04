# encoding=utf-8

from bs4 import BeautifulSoup, Tag
import xml.etree.ElementTree as ET
from kek_tools import kek_scan_index, kek_media_filename

num_errors = 0

class Company:
	name = ""
	url = ""
	shares = {}		# dict of unicode/Share
	titles = set()	# set of unicode

class Share:
	name = ""
	percent = 0.0

media = {}
companies = {}


def kek_parse_shares(shares_ul, owner, debug_prefix = ""):
	
	sum_percent = 0.0
	
	# all list entries
	for li in shares_ul.find_all("li", recursive=False):
		
		# first a is owner of the company
		a = li.find("a")
		if not type(a) is Tag: continue
		owned_by = a.text
		owned_by_url = a.get("href")
		if not type(owned_by) is unicode or not type(owned_by_url) is str: continue
		owned_by = owned_by.strip();
		
		# save company/url
		if not companies.has_key(owned_by):
			c = Company()
			c.name = owned_by
			c.url = owned_by_url
			c.shares = {}
			c.titles = set()
			companies[owned_by] = c
		
		# get percentage
		div = li.find("div", "anteil")
		if not type(div) is Tag: continue
		percent = div.text[0:len(div.text)-1]
		# check if it's a number
		try:
			tmp = float(percent)
			sum_percent += tmp
		except ValueError:
			# "0" for unknown, e.g. no information given
			percent = "0"
		
		# gotit
		print debug_prefix + percent + "% " + owned_by
		
		# save shares (owner is owned by owned_by)
		c = companies[owned_by]
		if not c.shares.has_key(owner):
			s = Share()
			s.name = owner
			s.percent = percent			
			c.shares[owner] = s
		
		# descent into owners of owners
		for ul in li.find_all("ul", "", recursive=False):
			if ul.text.find("publiziert:") < 0:
				kek_parse_shares(ul, owned_by, debug_prefix + " ")
	
	if not sum_percent == 0. and not sum_percent == 100.:
		print "WARNING percentage doesn't sum up right (" + str(sum_percent) + ")"
		
	return True
	

def kek_parse_media(local_name):
	"""Scans a media html file from kek-online.de
	and returns ... if succesful,
	False otherwise"""
		
	try:
		f = open(local_name, 'rt')
	except IOError:
		#print "ERROR opening " + local_name
		return False
	
	print "\nparsing " + local_name

	text = f.read()
	
	soup = BeautifulSoup(text)
	f.close();
	
	# the name seems always enclosed in the first <h2> tag
	m_name = soup.find("h2").text
	if not type(m_name) is unicode:
		print "ERROR did not find name (h2 tag)"; return False
	
	# owner and type info
	table = soup.find("table", "media-detail")
	if not type(table) is Tag:
		print "ERROR did not find media-table"; return False
	
	# first td is owner, second is type	
	td = table.find_all("td")
	if len(td) < 2:
		print "ERROR media-table is incomplete"; return False
	
	m_owner = td[0].find("a").text
	m_owner_url = td[0].find("a").get("href")
	m_type = td[1].text
	
	if not type(m_owner) is unicode or not type(m_owner_url) is str or not type(m_type) is unicode:
		print "ERROR got crap from media-table"
		print m_name + ", " + m_type + ", " + m_owner + ", " + m_owner_url; 
		return False
	
	m_owner_url = m_owner_url.encode("UTF-8")
	m_owner = m_owner.strip();
	m_type = m_type.strip();

	# got that :)
	print m_name + ", " + m_type + ", " + m_owner + ", " + m_owner_url

	# store the company info
	if not companies.has_key(m_owner):
		c = Company()
		c.name = m_owner
		c.url = m_owner_url
		c.shares = {}
		c.titles = set()
		companies[m_owner] = c

	companies[m_owner].titles.add(m_name)
	
	# the shares
	shares_ul = soup.find("ul", "js-shareholdings")
	if not type(shares_ul) is Tag:
		# ahh, top of the predetors
		return True

	kek_parse_shares(shares_ul, m_owner)
	
		
def kek_write_company_xml(filename):
	
	root = ET.Element("kek")
	comps = ET.SubElement(root, "companies")

	for c in companies.values():
		comp = ET.SubElement(comps, "company")
		comp.set("name", c.name)
		comp.set("url", c.url)
		for m in c.titles:
			title = ET.SubElement(comp, "title")
			title.set("name", m)
		for s in c.shares.values():
			share = ET.SubElement(comp, "share")
			share.set("name", s.name);
			share.set("p", s.percent);

	# store
	tree = ET.ElementTree(root)
	tree.write(filename, encoding="utf-8", xml_declaration=True)
	# and print
	ET.dump(root);


########################################################################

"""
if not companies.has_key("yes"):
	c = Company()
	c.name = "theyesman"
	companies["yes"] = c

s = Share()
s.name = "microsoft"
s.percent = 50.1

c = companies["yes"]

c.shares[s.name] = s

print companies["yes"].shares
"""

# scan the index file for all media titles
medien = kek_scan_index("./html/index.html")

# parse each downloaded media file
for i in medien.values():
	local_name = kek_media_filename(i)
	if kek_parse_media(local_name) == False:
		num_errors += 1

# print companies		
for c in companies.values():
	print "\ncompany: " + c.name
	for m in c.titles:
		print ' media: "' + m + '"'
	for s in c.shares.values():
		print ' share: ' + str(s.percent) + '% "' + s.name + '"'

kek_write_company_xml("./kek_owner.xml")
		
print str(num_errors) + " errors."
