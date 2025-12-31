require 'colorize'
require 'digest'

codes = %W[
      LDOb7UGhTi
      ImoFztWQCvxj
      BNCyODLfQkIl
      pWDWTEfURAdS
      rdMkyZhveeIv
      JyDQhSbkpyns
      NBlOWKLbTMgY
      #{'iW8UwOHpH8op'.reverse.gsub('p','q')}
    ]

answers = %w[
    76ec2408e8fe3f1753c25db51efd8eb3
    0e6aa7be1f68d930926d72b3741a145c
    7997a3b2941eab92c1c0345d5747b420
    186f842951c0dcfe8838af1e7222b7d4
    2bf84e54b95ce97aefd9fc920451fc45
    e09640936b3ef532b7b8e83ce8f125f4
    4873cf6b76f62ac7d5a53605b2535a0c
    d0c54d4ed7f943280ce3e19532dbb1a6
]

finds = []
codes.each do |c|
  finds << Digest::MD5.hexdigest(c.strip)
end

errors = finds - answers
unfound = (answers - (finds - errors))
total = answers.size

puts
puts "errors:  #{errors.size}".red.bold if errors.size > 0
if unfound.size > 0
  puts "Found: #{total - unfound.size} / #{total}".yellow.bold
else
  print "\nFound all #{total} Codes!".green.bold
  print " (with additional errors)".red.bold if(errors.size > 0)
end
