make real_clean
make

./p5exe < status.in > my.out
diff my.out status.out

./p5exe < cruise_ship.in > my.out
diff my.out cruise_ship.out

./p5exe < views.in > my.out
diff my.out views.out

./p5exe < torpedo.in > my.out
diff my.out torpedo.out

valgrind --tool=memcheck --leak-check=full ./p5exe < status.in > my.out