make real_clean
make

./p6exe < status.in > my.out
diff my.out status.out

./p6exe < cruise_ship.in > my.out
diff my.out cruise_ship.out

./p6exe < views.in > my.out
diff my.out views.out

./p6exe < torpedo.in > my.out
diff my.out torpedo.out

valgrind --tool=memcheck --leak-check=full ./p6exe < status.in > my.out