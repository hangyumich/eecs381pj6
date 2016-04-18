#make real_clean
#make

./p6exe < status.in > my.out
diff my.out status.out

./p6exe < cruise_ship.in > my.out
diff my.out cruise_ship.out

./p6exe < views.in > my.out
diff my.out views.out

./p6exe < torpedo.in > my.out
diff my.out torpedo.out

./p6exe < demo1_in.txt > my.out
diff my.out demo1_out.txt

./p6exe < demo2_in.txt > my.out
diff my.out demo2_out.txt

./p6exe < demo3_in.txt > my.out
diff my.out demo3_out.txt

# ./p6exe < demo4_in.txt > my.out
# diff my.out demo4_out.txt

valgrind --tool=memcheck --leak-check=full ./p6exe < status.in > my.out