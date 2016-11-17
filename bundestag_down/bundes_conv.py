import dircache
import os.path
import subprocess

def convert(fn, outpath):
	outf = os.path.join(outpath, fn.replace(".pdf", ".txt"))
	if os.path.exists(outpath) == False:
		print "creating directory " + outpath
		try: 
			os.makedirs(outpath)
		except OSError: 
			print "failed to create output dir"
	try:
		print "pdftotext -enc UTF-8 -nopgbrk " + fn + " " + outf
		subprocess.check_call(["pdftotext", "-enc", "UTF-8", "-nopgbrk", fn, outf])
	except OSError, subprocess.CalledProcessError:
		print "failed to convert the file"


def convert_dir(outpath):
	
	dirs = dircache.listdir(".")

	for i in dirs:
		if os.path.isdir(i):
			try:
				os.chdir(i)
				print "entered " + i
				convert_dir(outpath)
				os.chdir("..");
			except OSError:
				print "failed to enter " + i
		else:
			if i.find(".pdf") != -1:
				convert(i, outpath)
			else:
				print "skipping " + i


def convert_dirs(path, outpath):
	
	list = []
	dirs = dircache.listdir(path)

	for i in dirs:
		if os.path.isdir(i):
			try:
				os.chdir(i)
				print "entered " + i
				convert_dir(os.path.join(outpath, i))
				os.chdir("..")
			except OSError:
				print "failed to enter " + i



convert_dirs(".", os.path.join(os.path.abspath("utf8")))
