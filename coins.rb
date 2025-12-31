red = 2
corroded = 3
shiny = 5
concave = 7
blue = 9

coins = [red, corroded, shiny, concave, blue]

coins.each do |a|
  coins.each do |b|
    coins.each do |c|
      coins.each do |d|
        coins.each do |e|
          if [a,b,c,d,e].uniq == [a,b,c,d,e]
            if (a + b * c**2  + d**3 - e) == 399
              puts "found solution"
              puts "1. #{a}"
              puts "2. #{b}"
              puts "3. #{c}"
              puts "4. #{d}"
              puts "5. #{e}"
              exit 0
            end
          end
        end
      end
    end
  end
end
