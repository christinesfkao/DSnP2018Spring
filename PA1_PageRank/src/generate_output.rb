ds = [0.25, 0.45, 0.65, 0.85]
diffs = [0.1, 0.01, 0.001]

ds.each do |d|
  diffs.each do |diff|
    filename = "#{d.to_s[2..10]}_#{diff.to_s[2..10]}.txt"
    `echo "$(cat list.txt)\n*end*" | ruby main.rb #{diff} #{d} > outputs/result_#{filename}`
    `cp PageRank #{filename}.txt`
  end
end

`cp ReverseIndex ReverseIndex.txt`
