from kek_tools import *

if 1 == 1:

	# download the index-by-media html
	if kek_get_media_index() == False: print "failed.."; quit()

	# scan the index file for all media titles
	medien = kek_scan_media_index("./html/media_index.html")

	# download each title's html
	kek_download_media(medien)


if 1 == 1:
	
	# download the index-by-company html
	if kek_get_company_index() == False: print "failed.."; quit()

	# scan the index file for all media titles
	comps = kek_scan_company_index("./html/company_index.html")

	# download each company's html
	kek_download_companies(comps)


# There is a quirk!!
# Streubesitz is not part of the company index.
# We'll do it by hand here
if 1 == 1:
	url = "http://www.kek-online.de/no_cache/information/mediendatenbank.html?L=0%23&&c=14&"
	local_name = kek_company_filename(url)
	download(url, local_name)
