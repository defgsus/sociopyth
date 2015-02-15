# encoding=utf-8

# http://www.bundestag.de/bundestag/abgeordnete18/alphabet

#import os, sys
#lib_path = os.path.abspath('../modules')
#sys.path.append(lib_path)

from bs4 import BeautifulSoup, Tag
import xml.etree.ElementTree as ET

base_url = "http://www.bundestag.de"

bio18_prefix = "./html/bio18/"


##################### types ###############################


class Abgeordneter:
	def __init__(self):
		self.name = ""
		self.url = ""
		self.img_url = ""
		self.occupation = ""
		self.birth = ""
		self.wahlkreis = ""
		self.statements = []
		self.member = [] 		# list of list of str
		for i in range(0,4):
			self.member.append( list() )
	
	# needs all to be unicode	
	def __str__(self):
		r =   "name  [" + self.name + "]\n" \
			+ "occ.  [" + self.occupation + "]\n" \
			+ "birth [" + self.birth + "]"
		k = 0
		for j in self.member:
			k = k + 1
			for i in j:
				r += "\nmemb" + str(k) + " [" + i + "]";
			
		for i in self.statements:
			r += "\nstat. [" + i + "]"
		return r

	def addStatement(self, st):
		# who knows, we can split them later anyway
		self.statements.append( st )
		"""
			Adds str st to statements - splits statements at ';' 
		sts = st.split(";")
		for i in sts:
			i = i.strip()
			if len(i):
				self.statements.append( i )
		"""

	def concat(self, mlist):
		"""Concats entries ending with ',' """
		ret = []
		m = ""
		for i in mlist:
			if i.rfind(",") == len(i)-1:
				m += " " + i
			else:
				#if len(m): self.member3.append( m )
				ret.append( (m + " " + i).strip() )
				m = ""
		return ret


# XXX Does not accept unicode...
def bio_save_xml(fn, people):
	"""Saves a list of Abgeordneter to xml"""

	root = ET.Element("bundes-bio")
	
	# create company nodes
	for c in people:
		P = ET.SubElement(root, "person")
		P.set("name", c.name)
		P.set("url", c.url)
		P.set("img_url", c.img_url)
		P.set("birth", c.birth)
		P.set("wahlkreis", c.wahlkreis)
		P.set("occupation", c.occupation)
		for s in c.statements:
			S = ET.SubElement(P, "statement")
			S.set("value", s)
		k = 0
		for i in c.member:
			for j in i:
				M = ET.SubElement(P, "member")
				M.set("type", str(k));
				M.set("value", j);
			k += 1
			
	# store
	tree = ET.ElementTree(root)
	tree.write(fn, encoding="utf-8", xml_declaration=True)


###################### helper #############################

# complete address
def complete_url(url):
	if url.find(base_url) < 0:
		if (url.find("/") == 0):
			url = base_url + url
		else:
			url = base_url + "/" + url
	return url


def download(url, local_name):
	"""Copies the contents of a file from a given URL to a local file.
	url is, of course, the url of the source, which will be prefixed 
	with base_url if it's missing. 
	local_name is the filename of the local file to save the contents to. 
	Any directories will be created if necessary."""
	
	url = complete_url(url)
	
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



def bio18_scan_index(local_name):
	"""Scans the index-by-name html file and returns a list of urls"""
	
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


def bio18_filename(url):
	"""Returns a safe local filename from the bio url."""

	# scan for the database index to get a filename
	i = url.rfind("/")
	if i < 0: 
		print "Can't determine index from url " + url
		return False
	return bio18_prefix + url[i+1:] + ".html"



def bio18_download_index(fn):
	"""Downloads the index of bios of the 18th Wahlperiode."""
	
	print "downloading biography index"

	url = "http://www.bundestag.de/bundestag/abgeordnete18/alphabet"
	
	if download(url, fn) == False:
		print "failed";
		return False;
		
	return True;


def bio18_download(url):
	"""Downloads a bioghraphy to a local filename"""
	
	fn = bio18_filename(url)
	if fn == False: return False;
	
	if download(url, fn) == False: return False;
	
	return True


# get all types of "ausserparlamentarischer Tätigkeit"
member_types = set()

def bio18_scan_bio(url):
	"""Scans the biography html file.
	Returns False on Error, or class instance of Abgeordneter.
	"""
	
	local_name = bio18_filename(url)
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
	A.name = h1.text.strip()#.encode("UTF-8")
	A.url = complete_url(url)#.encode("UTF-8")
	
	#print "[" + A.name + "]"
	
	# image url
	div = soup.find("div", "bildDivPortrait")
	if type(div) is Tag:
		img = div.find("img")
		if type(img) is Tag:
			src = img.get("src")
			if not src is None:
				A.img_url = complete_url(src)#.encode("UTF-8")
				#print A.img_url
				
	# Wahlkreis
	for a in soup.find_all("a"):
		href = a.get("href")
		if not href == None:
			if "/wahlkreise" in href:
				if ":" in a.text:
					A.wahlkreis = a.text.strip()#.encode("UTF-8")
					#print A.wahlkreis

	# parse next tags after name
	idx = 0
	for i in h1.next_siblings:
		if type(i) is Tag:
			tex = i.text.strip()#.encode("UTF-8")
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
						tex = h3.text.strip()#.encode("UTF-8")
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


############################################


#if bio18_download_index(bio18_prefix + "index.html") == False: quit()

# scan index file
urls = bio18_scan_index(bio18_prefix + "index.html")

# download all bios
#for url in urls: bio18_download(url)

abgeordnete = []

#abgeordnete.append( bio18_scan_bio(urls[12]) )
#abgeordnete.append( bio18_scan_bio(urls[111]) )


# scan all bios
errors = 0
k = 0
for url in urls:
	k += 1
	print str(k) + "/" + str(len(urls))
	A = bio18_scan_bio(url)
	if A == False:
		errors += 1
	else:
		abgeordnete.append( A )
print str(errors) + " errors"		


bio_save_xml("./parlament_bio18.xml", abgeordnete)
print member_types

