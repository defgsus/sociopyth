# encoding=utf-8

# http://www.kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&m=735&mt=1,2,3,4&s=&f=0
# local_name = url.split(base_url)[-1]

import os, sys
lib_path = os.path.abspath('../modules')
sys.path.append(lib_path)

from bs4 import BeautifulSoup
from fun_with_sqlite import connect, create_schema, create_table

base_url = "http://www.kek-online.de/"

def download(url, local_name):
	"""Copies the contents of a file from a given URL to a local file."""
	
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
	

def kek_down():

	url = base_url + "no_cache/information/mediendatenbank.html?L=0"
	
	medien_index_url = url + "%23&mt=1,2,3,4&s=&f=0&pq=1"
	
	if download(medien_index_url, "./html/index.html") == False: return False
		
	return True


def kek_scan_index(local_name):
	
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
	
	soup = BeautifulSoup(text[i:])

	for td in soup.find_all("td", "mediaName"):
		for a in td.find_all("a"):
			print a.get("href") + " " + a.text
	
	f.close();
	
	
	
def test_sqlite():
	con, curs = connect("kek.sqlite")
	
	#create_table(con, curs, "kek", "( 
	
test_sqlite()

#kek_scan_index("./html/index.html")


# if kek_down() == False: print "failed.."
