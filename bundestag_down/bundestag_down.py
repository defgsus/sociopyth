
# script to download all "Drucksachen" from the Deutscher Bundestag

# dipbt.bundestag.de/doc/btd/01/000/0100001.pdf


base_url = "http://dipbt.bundestag.de/doc/btd/"

def get_name( period, docnum ):
	return base_url + '{:=02}'.format(period) + "/" + '{:=03}'.format(docnum/100) + "/" + '{:=02}'.format(period) + '{:=05}'.format(docnum) + ".pdf"
	
	
def download(url):
	"""Copy the contents of a file from a given URL
	to a local file.
	"""
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
	if webFile.geturl().find("navigation") != -1:
		return False;
	
	# filename & dir
	local_name = url.split(base_url)[-1]
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



for period in xrange(1, 20):
	
	start = 1
	if period == 1: start = 4683
	
	for docnum in xrange(start, 1<<24):
		if download( get_name(period, docnum) ) == False:
			print "failed"
			break
			
			
	


