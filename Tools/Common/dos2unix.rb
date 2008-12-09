#!/usr/bin/env ruby
# dos2unix.rb
# converts any type of line feeds to *nix format (\n)

ARGV.each do |filename|
	# Load original contents
	contents_file = File.open(filename, 'r')
	contents = contents_file.read()
	contents_file.close()
	
	# Replace newline sequences
	contents.gsub!(/\r\n?/, "\n")
	
	# Save contents, overwriting original
	replace_file = File.new(filename, "w+")
	replace_file.puts(contents)
	replace_file.close()
end