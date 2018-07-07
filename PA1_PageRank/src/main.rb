require 'pathname'
require 'readline'

pages = {} # empty hash ("maps" work as "hashtables" in Ruby) see line 29
DIFF = (ARGV[0] && ARGV[0].to_f) || 0.1
D_VALUE = (ARGV[1] && ARGV[1].to_f) || 0.25

# 1. read file
# 1.1 work with directories
page_pathnames = Pathname.new(File.join(File.dirname(__FILE__), 'web-search-files')).children
  # getwd by __FILE__
  # Pathname as an object (new) called |pathname|
  # use File.join to add directories together i.e. add the folder name
  # .children -> enter the folder to retrieve files inside

page_pathnames.each do |pathname|

  page_name = File.basename(pathname)  # get page0, page1 ... instead of entire directory
  content = File.read(pathname) # get stuff inside page0, page1 ...

  # 1.2 work with file content
    # use ".spilt()" cut file content into two parts with RegEx, separated by ----- and leftout
    content_parts = content.split(/^-+$/)
    # cut links by \n or ' ' and throw away blanks
    links = content_parts[0].split(/[\n ]/).reject { |link| link.empty? }
    # cut words by \n or ' ' and throw away blanks
    words = content_parts[1].split(/[\n ]/).reject { |word| word.empty? }

  # 1.3 put content into a selected data structure "pages", initiated line 3
    pages[page_name] = { links: links, words: words } # variable[key] = { value }

end

# 2. calculate pagerank
# 2.1 define method
# PageRank(page(0..n − 1), d, DIFF);
def page_rank(pages, d, stop_diff)
  pr = {} # initialize

  # line 1 in psuedo-code
  pages.each_pair do |page_name , _| # pick key
    pr[page_name] = 1.0 / pages.count # method "count" as in "size"
  end

  # line 2 in psuedo-code
  loop do
    # 2.1.2 initialization
    diff = 0.0
    pr_next = {} # save latest pr after calculations, replace

    pages.each_pair do |page_name, _|
      # 2.1.2 selection for outgoing links
      links_out = pages.select do |_ , page_data| # pick value
        page_data[:links].include?(page_name) # method "include" as in "search"
      end
      # 2.1.3 calculate new pr
      ratios = links_out.keys.map { |page_name| pr[page_name] / pages[page_name][:links].count }
      # links is a value of a hash named pages (line 29)
      # 2.1.4 save latest pr
      pr_next[page_name] = ( 1.0 - d ) / pages.count + d * ratios.inject(0, :+) #ratios.sum
      # 2.1.5 calculate new diff
      diff += (pr_next[page_name] - pr[page_name]).abs
    end

    # 2.1.6 renew pr
    pr = pr_next
    break if diff < stop_diff # line 3 in psuedo-code
  end
  return pr
end # line 4 in psuedo-code

# 2.2 calculation
pr_hash = page_rank(pages, D_VALUE, DIFF) # pr_hash is a hash, keys: pagename, values: pr

# 2.3 sort pagerank
pagenames = pages.keys
pagenames.sort! { |a, b| pr_hash[b] <=> pr_hash[a] }
# order from highest to lowest
# sort! is inplace

# 2.4 write PageRank file

open('PageRank', 'w') do |f| # open file called "PageRank"
  pagenames.each do |page_name|
    f << "#{page_name}\t"
    f << "#{pages.select { |_ , data| data[:links].include?(page_name)}.count}\t"
    f << "#{pr_hash[page_name].to_s[1..9]}\n" # to_string, pick first to ninth char
    end
end

# 3. set reverse index

# 3.1 build word hash
word_hash = {} # initialize hash -  keys: word, value: an array of pagenames
# go over all pages (iterate through)
pages.each_pair do |page_name, page_data|
  # pages[page_name] = { links: links, words: words } see line 29
  page_data[:words].each do |word|
    word_hash[word] = [] unless word_hash[word] # defifnot, initialize
    # for each word (key), push pagename (value) into array
    word_hash[word].push(page_name)
  end
end

# 3.2 delete repeated pagenames for each word
word_hash.each_pair do |_, page_names|
  page_names.uniq!
  page_names.sort! # sort page_names
end

# 3.3 sort words in alphabetical order
# not inplace, turn into array
# hash has no sequence
word_sorted = word_hash.sort

# 3.4 write ReverseIndex file
open('ReverseIndex', 'w') do |f| # open file called "ReverseIndex"
  word_sorted.each do |w_ps| # pages used, so abbreviate
    w = w_ps.first
    ps = w_ps.last
    f << "#{w}\t"
    f << "#{ps.join(' ')}\n"
  end
end

# 4. build search engine
# 4.1 sort pages by PageRank
#

loop do
  input = Readline.readline("Enter Word: ", true)
  next unless input
  input.strip!
  break if input == '*end*'
  input_words = input.split(' ') # saved as array

  if input_words.count == 0
  elsif input_words.count == 1
    unless word_hash[input] # error handling for nil
      puts 'none'
      next
    end
    result = word_hash[input].sort { |a, b| pr_hash[b] <=> pr_hash[a]}
    puts result[0..9].join(' ') # get top ten
  else # input_words.count >= 2
    and_result = pages.keys # initialize array with all words
    or_result = [] # initialize empty array

    input_words.each do |input_word| # repeated pagenames won't appear twice
      and_result &= (word_hash[input_word] || []) # error handling if &= nil
      or_result |= (word_hash[input_word] || []) # error handling if |= nil
    end
    # sort by PageRank
    and_result.sort! { |a, b| pr_hash[b] <=> pr_hash[a] }
    or_result.sort! { |a, b| pr_hash[b] <=> pr_hash[a] }

    puts "AND (#{input}) #{and_result.count.zero? ? 'none' : and_result[0..9].join(' ')}" # get top ten
    puts "OR (#{input}) #{or_result.count.zero? ? 'none' : or_result[0..9].join(' ')}" # get top ten
  end
end
