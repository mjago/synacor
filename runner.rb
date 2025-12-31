require 'open3'
require 'colorize'

@inputs=[
  'take tablet',
  'use tablet',
  'doorway',
  'north',
  'north',
  'bridge',
  'continue',
  'down',
  'west',
  'passage',
  'ladder',
  'east',
  'south',
  'west',
  'west',
  'south',
  'north',
  'take can',
  'west',
  'ladder',
  'cavern',
  'east',
  'east',
  'take empty lantern',
  'use can',
  'west',
  'west',
  'passage',
  'darkness',
  'use lantern',
  'continue',
  'west',
  'west',
  'west',
  'west',
  'north',
  'take red coin',
  'north',
  'east',
  'take concave coin',
  'down',
  'take corroded coin',
  'up',
  'west',
  'west',
  'take blue coin',
  'up',
  'take shiny coin',
  'down',
  'east',
  'use blue coin',
  'use red coin',
  'use shiny coin',
  'use concave coin',
  'use corroded coin',
  'north',
  'take teleporter',
  'use teleporter',
  'take business card',
  'look business card',
  'take strange book',
  'look strange book',
  'outside',
  '~ fix teleporter',
  'use teleporter',
  'inv',
  'north',
  'north',
  'north',
  'north',
  'north',
  'north',
  'north',
  'north',
  'north',
  'take orb',
  'look orb',
  'look orb',

  'north',
  'east',
  'east',
  'north',
  'west',
  'south',
  'east',
  'east',
  'west',
  'north',
  'north',
  'east',
  'vault',
  'take mirror',
  'look mirror',
  'use mirror',
  'look'
]

cmd = %w[stdbuf -oL /home/martyn/c/vm/out /home/martyn/c/vm]
Open3.popen3(*cmd) do |stdin, stdout, stderr, wait_thr|
  stdin.sync  = true
  stdout.sync = true
  stderr.sync = true

  loop do
    break if wait_thr.join(0)

    ready = IO.select([stdout, stderr], nil, nil, 0.1)
    next unless ready

    ready[0].each do |io|
      begin
        data = io.read_nonblock(1024, exception: false)
        next if data == :wait_readable || data.nil?

        if io == stdout
          data.each_line do |line|
            line.chomp!
            unless line.empty?
              puts line.cyan.bold
              if line.include?("What do you do?")
                puts "#{@inputs.first}".yellow.bold
                stdin.puts @inputs.first
                @inputs.shift
                exit(0) if (@inputs.empty?)
              end
            end
          end
        else
          data.each_line { |line| warn "ERR: #{line.chomp}" }
        end
      rescue EOFError
        raise "❌ error"
      end
    end
  end

  puts "STATUS: #{wait_thr.value.exitstatus}"
end
