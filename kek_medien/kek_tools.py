# encoding=utf-8

# http://www.kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&m=735&mt=1,2,3,4&s=&f=0

#import os, sys
#lib_path = os.path.abspath('../modules')
#sys.path.append(lib_path)

from bs4 import BeautifulSoup, Tag
#from fun_with_sqlite import connect, create_schema, create_table
import xml.etree.ElementTree as ET


class Company:
	name = ""
	url = ""
	address = ""
	remarks = ""
	shares = {}		# dict of unicode/Share
	titles = set()	# set of unicode

class Share:
	name = ""
	percent = 0.0

class Title:
	name = ""
	url = ""
	mtype = "" 		# media type



base_url = "http://www.kek-online.de/"

def download(url, local_name):
	"""Copies the contents of a file from a given URL to a local file.
	url is, of course, the url of the source, which will be prefixed 
	with base_url if it's missing. 
	local_name is the filename of the local file to save the contents to. 
	Any directories will be created if necessary."""
	
	# complete address
	if url.find(base_url) < 0:
		url = base_url + url
	
	import urllib2
	import os.path
	print "fetching " + url
	
	# get url
	
	try: 
		webFile = urllib2.urlopen(url)
	except urllib2.HTTPError:
		print "failed";
		return False
		
	if webFile.getcode() != 200:
		return False
	# stupid t-online catch 
	if webFile.geturl().find("navigation") != -1:
		return False;
	
	# filename & dir
	local_dir = os.path.dirname(local_name)
	if os.path.exists(local_dir) == False:
		if os.makedirs(local_dir) == False:
			print "failed creating dir " + local_dir
			return False
	
	# save
	localFile = open(local_name, 'w')
	localFile.write(webFile.read())
	webFile.close()
	localFile.close()
	return True
	

################### media/title index #########################

def kek_get_media_index():
	"""Downloads the index page of the Mediendatenbank,
	listing all media titles.
	Returns False or True."""

	url = base_url + "no_cache/information/mediendatenbank.html?L=0"
	
	medien_index_url = url + "%23&mt=1,2,3,4&s=&f=0&pq=1"
	
	if download(medien_index_url, "./html/media_index.html") == False: return False
		
	return True


def kek_scan_media_index(local_name):
	"""Scans the index-by-media html file from kek-online.de
	and returns a dictionary with media-name/url pairs if succesful,
	False otherwise"""
	
	try:
		f = open(local_name, 'rt')
	except IOError:
		print "ERROR opening " + local_name
		return False
	
	text = f.read()#.split('<table class="media-table">')
	
	# beginning of table
	i = text.find('<table class="media-table">')
	if i < 0 : 
		print "ERROR index table not found in " + local_name
		return False

	medien = {}
	soup = BeautifulSoup(text[i:])

	# get each media owner
	for td in soup.find_all("td", "mediaName"):
		for a in td.find_all("a"):
			#print a.get("href") + " " + a.text
			medium = a.text.encode('UTF-8')
			medium_url = a.get("href").encode('UTF-8') 
			medien[medium] = medium_url
	
	f.close();
	
	print "found " + str(len(medien)) + " media titles"
	return medien


def kek_media_filename(media_url):
	"""Returns a safe local filename from the media url,
	or False if the url is messed-up."""

	# scan for the database index to get a good filename
	i = media_url.find("&m=")
	j = media_url[i+3:].find("&")
	if i < 0 or j < 0: 
		print "Can't determine index from url " + media_url
		return False
	i += 3
	return "./html/media_" + media_url[i:i+j] + ".html"


def kek_download_media(medien):
	"""Downloads all the urls in the dictionary's values."""

	print "downloading all media htmls"

	for i in medien.values():
		local_name = kek_media_filename(i)
		if i == False: continue
		if download(i, local_name) == False: continue
	
	return True




################### company/owner index #########################

def kek_get_company_index():
	"""Downloads the index page of the Mediendatenbank,
	listing all media titles.
	Returns False or True."""

	url = base_url + "no_cache/information/mediendatenbank.html?L=0"
	
	medien_index_url = url + "&mt=1,2,3&s=&searchCompanies=1&f=0&pq=1"
	
	if download(medien_index_url, "./html/company_index.html") == False: return False
		
	return True


def kek_scan_company_index(local_name):
	"""Scans the index-by-company html file from kek-online.de
	and returns a dictionary with company-name/url pairs if succesful,
	False otherwise"""
	
	try:
		f = open(local_name, 'rt')
	except IOError:
		print "ERROR opening " + local_name
		return False
	
	text = f.read()
	
	# beginning of table
	i = text.find('<table class="media-detail')
	if i < 0 : 
		print "ERROR company index table not found in " + local_name
		return False

	companies = {}
	soup = BeautifulSoup(text[i:])

	# get each media owner
	# The td classes are not very helpful, but
	# by cutting the text above to the company-table
	# and assuming there are no other td's down the page
	# (which is the case on Feb/2015) we can just read them all
	for td in soup.find_all("td"):
		for a in td.find_all("a"):
			#print a.get("href") + " " + a.text
			comp = a.text.encode('UTF-8')
			comp_url = a.get("href").encode('UTF-8') 
			companies[comp] = comp_url
	
	f.close();
	
	print "found " + str(len(companies)) + " companies"
	return companies


def kek_company_filename(company_url):
	"""Returns a safe local filename from the company url,
	or False if the url is messed-up."""

	# scan for the database index to get a good filename
	i = company_url.find("&c=")
	j = company_url[i+3:].find("&")
	if i < 0 or j < 0: 
		print "Can't determine index from url " + company_url
		return False
	i += 3
	return "./html/company_" + company_url[i:i+j] + ".html"

def kek_download_companies(companies):
	"""Downloads all the urls in the dictionary's values."""

	print "downloading all company htmls"

	for i in companies.values():
		local_name = kek_company_filename(i)
		if i == False: continue
		if download(i, local_name) == False: continue
	
	return True


################### xml ####################


def kek_write_xml(companies, titles, filename):
	"""Writes the contents of companies and titles to an xml file.
	companies must be a dict of unicode/Company.
	titles must be a dict of unicode/Title."""
	
	root = ET.Element("kek")
	
	# create company nodes
	comps = ET.SubElement(root, "companies")
	for c in companies.values():
		comp = ET.SubElement(comps, "company")
		comp.set("name", c.name)
		comp.set("url", c.url)
		comp.set("address", c.address)
		comp.set("remarks", c.remarks)
		for m in c.titles:
			title = ET.SubElement(comp, "title")
			title.set("name", m)
		for s in c.shares.values():
			share = ET.SubElement(comp, "share")
			share.set("p", s.percent);
			share.set("name", s.name);
			
	# create title nodes
	tits = ET.SubElement(root, "titles")
	for c in titles.values():
		t = ET.SubElement(tits, "title-desc")
		t.set("name", c.name)
		t.set("url", c.url)
		t.set("type", c.mtype)

	# store
	tree = ET.ElementTree(root)
	tree.write(filename, encoding="utf-8", xml_declaration=True)
	# and print
	ET.dump(root);


def kek_read_xml(filename):
	"""Reads the contents of the kek data xml file.
	Returns a dict of unicode/Company and unicode/Title. 
	No error checking here - i must trust myself.."""
	
	ret_comp = {}	
	ret_title = {}
	
	tree = ET.parse(filename)
	root = tree.getroot()
	
	# go through all company tags
	for comp in root.iter("company"):
		
		# get a key/value dict with all attributes
		attr = comp.attrib
		
		# create the runtime structure
		c = Company()
		c.name = attr["name"]
		c.url = attr["url"]
		c.address = attr["address"]
		c.remarks = attr["remarks"]
		c.titles = set()
		c.shares = {}
		
		# get all media titles
		for media in comp.iter("title"):
			c.titles.add(media.attrib["name"])
		
		# get owned companies	
		for share in comp.iter("share"):
			s = Share()
			s.name = share.attrib["name"]
			s.percent = float(share.attrib["p"])
			c.shares[s.name] = s
		
		# store in dict
		ret_comp[c.name] = c

	# go through all title tags
	for tit in root.iter("title-desc"):
		
		# get a key/value dict with all attributes
		attr = tit.attrib
		
		# create the runtime structure
		t = Title()
		t.name = attr["name"]
		t.url = attr["url"]
		t.mtype = attr["type"]
		
		ret_title[t.name] = t;
		
	return ret_comp, ret_title
