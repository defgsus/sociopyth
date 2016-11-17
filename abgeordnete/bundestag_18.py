# encoding=utf-8

# http://www.thueringer-landtag.de/landtag/abgeordnete-und-fraktionen/abgeordnete/


from bs4 import BeautifulSoup, Tag
from tools import Abgeordneter, get_complete_url, download_file, bio_save_xml


########################## helper ######################################

base_url = "http://www.bundestag.de"
html_dir = "./html/bundestag18/"

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
	
	urls = []
	soup = BeautifulSoup(text)

	# get each media owner
	for div in soup.find_all("div", "linkIntern"):
		for a in div.find_all("a"):
			href = a.get("href");
			if "/biografien/" in href:
				print a.text.strip()
				urls.append( href )
	
	print "found " + str(len(urls)) + " biography urls"
	return urls



member_types = set()

def parse_bio(url):
	"""Parses a biography html and returns a class of Abgeordneter, or False."""

	local_name = get_filename(url)
	if local_name == False: return False
	
	try:
		f = open(local_name, 'rt')
	except IOError:
		print "ERROR opening " + local_name
		return False
	
	text = f.read()
	f.close();
	
	soup = BeautifulSoup(text)
	
	bio = soup.find("div", "standardBox")
	if not type(bio) is Tag: print "ERROR: bio-box not found"; return False
	
	# name
	h1 = bio.find("h1")
	if not type(h1) is Tag: print "ERROR: h1 name not found"; return False
	
	# create a structure
	A = Abgeordneter()
	A.url = get_complete_url(base_url, url)
	A.gremium = u"Bundestag"
	A.period = u"18"
	
	texts = h1.text
	idx = texts.rfind(",")
	if idx < 0:
		A.name = texts.strip()
	else:
		A.name = texts[:idx].strip()
		A.party = texts[idx+1:].strip()
	
	#print "[" + A.name + "]"
	
	# image url
	div = soup.find("div", "bildDivPortrait")
	if type(div) is Tag:
		img = div.find("img")
		if type(img) is Tag:
			src = img.get("src")
			if not src is None:
				A.img_url = get_complete_url(base_url, src)
				#print A.img_url
				
	# Wahlkreis
	for a in soup.find_all("a"):
		href = a.get("href")
		if not href == None:
			if "/wahlkreise" in href:
				if ":" in a.text:
					A.wahlkreis = a.text.strip()
					#print A.wahlkreis

	# parse next tags after name
	idx = 0
	for i in h1.next_siblings:
		if type(i) is Tag:
			tex = i.text.strip()
			if len(tex):
	#			print "---[" + tex + "]"
				if idx == 0: A.occupation = tex
				if idx == 1: A.birth = tex
				if idx > 1: A.addStatement(tex) 
				idx = idx + 1

	# proceed with memberships
	for i in bio.next_siblings:
		if type(i) is Tag:
			# membership box
			if "Mitglied" in i.text:
				for div in i.find_all("div"):
					for h3 in div.find_all("h3"):
						tex = h3.text.strip()
						member_types.add(tex)
						#print "###[" + tex + "]"
						# ordentliches Mitglied
						if "Ordentlich" in tex:
							memb = A.member[0]
						else: 
							# stellvertretendes Mitglied
							if "Stellver" in tex:
								memb = A.member[1]
							# Funktion in Firmen, Verbänden, etc...
							else: 
								memb = []
														
								for j in h3.next_siblings:
									if type(j) is Tag and j.name == "p":
										if j.get("class") == None or not "kleinAbstand" in j.get("class"):
											memb.append( j.text.strip() )#.encode("UTF-8") )
								if "vor der Mitgliedschaft" in tex:
									A.member[3] += A.concat(memb);
								else:
									A.member[2] += A.concat(memb);
								break;
								
						# scan ordentlich & stellvertretend
						for j in h3.next_siblings:
							if type(j) is Tag:
								#print j.name
								if j.name == "h3": break;
								if j.name == "ul":
									for k in j.find_all("li"):
										memb.append( k.text.strip() )#.encode("UTF-8") )
									
	#print A
	return A


########################################################################

do_download = False

# download index file
if do_download == True:
	if download_file(base_url + "/bundestag/abgeordnete18/alphabet", html_dir + "index.html") == False: quit()

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
	print "parsing " + url
	a = parse_bio(url)
	if a is False: 
		print "in " + get_filename(url)
		errors += 1
	else:
		#print unicode(a)
		people.append( a )
	#break;
	
print str(errors) + " errors"
print "membership types: " + str(member_types)

bio_save_xml("./xml/bundestag18.xml", people)
