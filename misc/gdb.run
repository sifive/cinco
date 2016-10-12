target extended-remote localhost:3333
display/i $pc
define nstepi
 set $foo = $arg0
 while ($foo--)
  stepi
 end
end
set $pc = 0x80000000
set pagination off

