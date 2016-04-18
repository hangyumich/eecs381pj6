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

./p6exe < S1_1.in > my.out
diff my.out S1_1.out

./p6exe < S1_2.in > my.out
diff my.out S1_2.out

./p6exe < S1_3.in > my.out
diff my.out S1_3.out

./p6exe < S1_4.in > my.out
diff my.out S1_4.out

./p6exe < S1_5.in > my.out
diff my.out S1_5.out

./p6exe < S2_1.in > my.out
diff my.out S2_1.out

./p6exe < S2_2.in > my.out
diff my.out S2_2.out


valgrind --tool=memcheck --leak-check=full ./p6exe < status.in > my.out