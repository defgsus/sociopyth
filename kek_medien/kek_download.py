from kek_tools import *

# download the index-by-media html
if kek_get_index() == False: print "failed.."; quit()
# scan the index file for all media titles
medien = kek_scan_index("./html/index.html")
# download each title's html
kek_download_media(medien)


