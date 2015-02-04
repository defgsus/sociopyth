# encoding=utf-8

# http://www.kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&m=735&mt=1,2,3,4&s=&f=0
# local_name = url.split(base_url)[-1]

#import os, sys
#lib_path = os.path.abspath('../modules')
#sys.path.append(lib_path)

from bs4 import BeautifulSoup, Tag
#from fun_with_sqlite import connect, create_schema, create_table
import xml.etree.ElementTree as ET

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
	


def kek_get_index():
	"""Downloads the index page of the Mediendatenbank,
	listing all media.
	Returns False or True."""

	url = base_url + "no_cache/information/mediendatenbank.html?L=0"
	
	medien_index_url = url + "%23&mt=1,2,3,4&s=&f=0&pq=1"
	
	if download(medien_index_url, "./html/index.html") == False: return False
		
	return True



def kek_scan_index(local_name):
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

