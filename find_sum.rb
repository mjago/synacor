require 'colorize'

$stdout.sync = true
$stderr.print "Solving...".yellow.bold

TARGET = 30
VAULT  =
  [
    ['*'  , '8', '-' , '1'],
    ['4'  , '*', '11', '*'],
    ['+'  , '4', '-' , '18'],
    ['orb', '-', '9' , '*'],
  ]
DX = [-1, 0, 1, 0]
DY = [0, 1, 0, -1]

start_pos  = [3, 0]
start_sum  = 22
start_op   = ''

queue   = []
visited = {}
parent   = {}
distance = {}

start_state = [start_pos, start_sum, start_op]
queue << start_state
visited[start_state] = true
distance[start_state] = 0
now = Time.now

def in_bounds?(x, y)
  x.between?(0, 3) && y.between?(0, 3)
end

def tile_at(pos)
  VAULT[pos[0]][pos[1]]
end

def direction(from_pos, to_pos)
  fx, fy = from_pos
  tx, ty = to_pos
  dx = tx - fx
  dy = ty - fy

  case [dx, dy]
  when [-1,  0] then 'north'
  when [ 1,  0] then 'south'
  when [ 0,  1] then 'east'
  when [ 0, -1] then 'west'
  else
    $stderr.puts "❌ Error: invalid direction!";
  end
end


moves = 0

until queue.empty?
  moves += 1
  pos, sum, op = queue.shift
  x, y = pos

  sleep 0.01 if (pos == [0, 3])

  if (pos == [0, 3]) && (sum == TARGET)
    $stderr.puts "\rSolved in #{Time.now - now} seconds.".yellow.bold

    goal_state = [pos, sum, op]
    path = []
    state = goal_state

    while state
      path << state
      state = parent[state]
    end

    path.reverse!
    directions = []
    path.each_cons(2) do |(pos1, _sum1, _op1), (pos2, _sum2, _op2)|
      directions << direction(pos1, pos2)
    end

    puts "\n*   *   *   PATH   *   *   *\n".green.bold
    directions.map {|a| puts a.green.bold}
    puts
    exit
  end

  4.times do |i|
    nx = x + DX[i]
    ny = y + DY[i]
    npos = [nx, ny]
    next unless in_bounds?(nx, ny)

    cell = tile_at(npos)

    nsum = sum
    nop  = op

    if cell == 'orb'
      nsum = 22
      nop  = ''
    elsif ['*', '+', '-'].include?(cell)
      nop = cell
    else
      val = cell.to_i
      case nop
      when '+' then nsum += val
      when '-' then nsum -= val
      when '*' then nsum *= val
      when ''
        nsum = sum
      end
    end

    state = [npos, nsum, nop]

    next if visited[state]

    visited[state] = true
    parent[state] = [pos, sum, op]
    distance[state] = distance[[pos, sum, op]] + 1
    queue << state
  end
end

puts 'No solution'
