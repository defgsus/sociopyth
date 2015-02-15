# encoding=utf-8

# http://www.thueringer-landtag.de/landtag/abgeordnete-und-fraktionen/abgeordnete/


from bs4 import BeautifulSoup, Tag
from tools import Abgeordneter, get_complete_url, download_file, bio_save_xml


########################## helper ######################################

base_url = "http://www.thueringer-landtag.de"
html_dir = "./html/th/"

def get_filename(url):
	"""Returns a local filename for the bio url, or False"""
	i = url.rfind("/")
	j = url[:i].rfind("/")
	if i < 0 or j < 0: 
		print "Can't determine index from url " + url
		return False
	return html_dir + url[j+1:i] + ".html"



########################## parsing #####################################

def parse_index(local_name):
	"""Parses the index file, returns list of urls, or False"""
	
	try:
		f = open(local_name, 'rt')
	except IOError:
		print "ERROR opening " + local_name
		return False
	
	text = f.read()
	f.close();
	
	soup = BeautifulSoup(text)
	
	urls = []
	
	for a in soup.find_all("a"):
		href = a.get("href")
		if type(href) is str:
			if "/abgeordnete/biografien/daten" in href:
				urls.append( get_complete_url(base_url, a.get("href")) )
	
	return urls


def parse_bio(url):
	"""Parses a biography html and returns a class of Abgeordneter, or False."""

	local_name = get_filename(url)
	
	try:
		f = open(local_name, 'rt')
	except IOError:
		print "ERROR opening " + local_name
		return False
	
	text = f.read()
	f.close();
	
	soup = BeautifulSoup(text)
	
	cont = soup.find("div", id = "content")
	if not type(cont) is Tag: print "ERROR missing content div"; return False
		
	#print cont
		
	A = Abgeordneter()
	A.url = url
	A.gremium = u"Landtag Thüringen"
	A.period = u"6"
	
	# main entry
	h2 = cont.find("h2", "alternativFont")
	if not type(h2) is Tag: print "ERROR missing main h2"; return False
	A.name = h2.text
		
	# image url
	img = cont.find("img")
	if type(cont) is Tag: 
		A.img_url = get_complete_url(base_url, img.get("src"))

	# info below name
	for i in h2.next_siblings:
		if type(i) is Tag: 
			if i.name == "p" and len(i.text):
				# party
				if "Fraktion:" in i.text:
					# split into party and Wahlkreis
					text = i.text
					text = text[10:] # remove "Fraktion: "
					idx = text.find(",")
					if idx < 0:
						A.party = text
					else:
						A.party = text[:idx]
						A.wahlkreis = text[idx+1:].strip()
					continue
				# occupation
				else: 
					if not len(A.occupation):
						# TODO: They use <br/>s to split different statements
						A.occupation = i.text
						continue
					else:
						if len(A.party): break
	# verify
	#if not len(A.occupation):
	#	print "ERROR missing p after h2"; return False
	if not len(A.party):
		print "ERROR missing Fraktion"; return False
		
	# general data
	for h2 in cont.find_all("h2"):
		# birth 
		if "nliche Daten" in h2.text:
			for i in h2.next_siblings:
				if type(i) is Tag:
					if i.name == "ul":
						for li in i.find_all("li"):
							if len(A.birth): A.birth += "\n"
							A.birth += li.text
						break
		# memberships
		if "Funktionen" in h2.text:
			for i in h2.next_siblings:
				if type(i) is Tag:
					if i.name == "ul":
						for li in i.find_all("li"):
							A.member[2].append(li.text)

	# biography							
	dl = cont.find("dl", "vita")
	if type(dl) is Tag:
		first = ""
		for i in dl.children:
			if type(i) is Tag:
				if i.name == "dt": first = i.text
				if i.name == "dd":
					A.statements.append(first + " " + i.text)
					first = ""				
	
	return A
	

########################################################################

do_download = False

# download index file
if do_download == True:
	if download_file(base_url + "/landtag/abgeordnete-und-fraktionen/abgeordnete/", html_dir + "index.html") == False: quit()

# parse index file for bios
urls = parse_index(html_dir + "index.html") 
if urls == False: quit()

# download all bios
if do_download == True:
	for url in urls:
		fn = get_filename(url)
		if fn == False: quit()
		download_file(get_complete_url(base_url, url), fn)
	
# parse all biographies
errors = 0
people = []
for url in urls:
	a = parse_bio(url)
	if a is False: 
		print "in " + get_filename(url)
		errors += 1
	else:
		#print unicode(a)
		people.append( a )
	
print str(errors) + " errors"

bio_save_xml("./xml/landtag_th6.xml", people)
