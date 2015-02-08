# encoding=utf-8

"""
Once files have been downloaded with kek_download.py,
this prog creates an xml from the htmls.
"""


from bs4 import BeautifulSoup, Tag
import xml.etree.ElementTree as ET
from kek_tools import kek_scan_media_index, kek_scan_company_index, kek_media_filename, kek_company_filename, kek_write_xml, Company, Share, Title

num_errors = 0

titles = {}			# dict unicode/Title
companies = {}		# dict unicode/Company



##################### media html parsing ###############################

def kek_parse_shares(shares_ul, owner, debug_prefix = ""):
	"""Recursive function that parses the shares ul table.
	"""
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
			# some lists are not shares but media titles
			if ul.text.find("publiziert:") < 0 and ul.text.find("veranstaltet:") < 0:
				if kek_parse_shares(ul, owned_by, debug_prefix + " ") == False:
					return False
	
	if not sum_percent == 0. and not sum_percent == 100.:
		print "WARNING percentage doesn't sum up right (" + str(sum_percent) + ")"
		
	return True
	

def kek_parse_media(media_url, local_name):
	"""Scans a media html file from kek-online.de
	and returns True if succesful, False otherwise. 
	Info is added to the global vars companies and titles.
	"""
		
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

	# store the media/title info
	if not titles.has_key(m_name):
		t = Title()
		t.name = m_name
		t.url = media_url
		t.mtype = m_type
		titles[m_name] = t

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
	
	# descent into the shares table (which is recursive)
	if len(shares_ul["class"]) == 1:
		return kek_parse_shares(shares_ul, m_owner)

	return True

	
################### company html parsing ###############################

def kek_parse_company(company_url, local_name):
	"""Scans a company html file from kek-online.de
	and returns True if succesful, False otherwise. 
	Info is added to the global dict companies.
	"""
		
	try:
		f = open(local_name, 'rt')
	except IOError:
		print "ERROR opening " + local_name
		return False
	
	print "\nparsing " + local_name

	text = f.read()
	
	soup = BeautifulSoup(text)
	f.close();
	
	# the name seems always enclosed in the first <h2> tag
	m_name = soup.find("h2").text
	if not type(m_name) is unicode:
		print "ERROR did not find name (h2 tag)"; return False
	
	# details about this entity
	m_name = m_name.strip()
	m_address = ""
	m_remarks = ""
	tab = soup.find("table", "media-detail")
	if type(tab) is Tag:
		
		for tag in tab.children:
			if type(tag) is Tag:
				if tag.text.find("Adresse:") >= 0:
					m_address = tag.text[9:].strip()
				if tag.text.find("Bemerkungen:") >= 0:
					m_remarks = tag.text[13:].strip()
	
	# got this :)
	print "[" + m_name + "]\n[" + m_address + "]\n[" + m_remarks + "]"

	# store the company info
	if not companies.has_key(m_name):
		c = Company()
		c.name = m_name
		c.url = company_url
		c.shares = {}
		c.titles = set()
		companies[m_name] = c
	else: c = companies[m_name]
	
	c.address = m_address
	c.remarks = m_remarks
	
	
	# the shares
	shares_ul = soup.find("ul", "js-shareholdings")
	if not type(shares_ul) is Tag:
		# ahh, top of the predetors
		return True
		
	# descent into the shares table (which is recursive)
	if len(shares_ul["class"]) == 1:
		return kek_parse_shares(shares_ul, m_name)
		
	return True
			

########################################################################


# scan the index file for all company urls
comps = kek_scan_company_index("./html/company_index.html")

# parse each downloaded company html
for i in comps.values():
	#i = "http://www.kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&c=192&mt=-1&s=RTL&f=0"
	#i = "http://www.kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&&c=3847&"
	#i = "http://www.kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&&c=200&"
	#i = "http://www.kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&&c=195&"
	#i = "http://www.kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&&c=14&"
	local_name = kek_company_filename(i)
	if local_name == False:
		num_errors += 1
	else:
		if kek_parse_company(i, local_name) == False:
			num_errors += 1


# scan the index file for all media titles
medien = kek_scan_media_index("./html/media_index.html")

# parse each downloaded media file
for i in medien.values():
	local_name = kek_media_filename(i)
	if local_name == False:
		num_errors += 1
	else:
		if kek_parse_media(i, local_name) == False:
			num_errors += 1

kek_write_xml(companies, titles, "./kek_data.xml")



# print companies		
for c in companies.values():
	print "\ncompany: " + c.name
	print ' address: "' + c.address + '"'
	print ' remarks: "' + c.remarks + '"'
	for m in c.titles:
		print ' media: "' + m + '"'
	for s in c.shares.values():
		print ' share: ' + str(s.percent) + '% "' + s.name + '"'

print str(num_errors) + " errors."
