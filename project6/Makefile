CC = g++
LD = g++

CFLAGS = -c -pedantic-errors -std=c++14 -Wall -fno-elide-constructors -g
LFLAGS = -pedantic-errors -Wall

OBJS = p6_main.o Controller.o Island.o Ship.o Tanker.o View.o Views.o Cruise_ship.o Warships.o Model.o Ship_factory.o Track_base.o Geometry.o Navigation.o Sim_object.o Utility.o Group.o Commandable.o Refuel_ship.o
PROG = p6exe

default: $(PROG)

$(PROG): $(OBJS) 
	$(LD) $(LFLAGS) $(OBJS) -o $(PROG)

p6_main.o: p6_main.cpp *.h
	$(CC) $(CFLAGS) p6_main.cpp

Controller.o: Controller.cpp *.h
	$(CC) $(CFLAGS) Controller.cpp

Island.o: Island.cpp *.h
	$(CC) $(CFLAGS) Island.cpp

Ship.o: Ship.cpp *.h
	$(CC) $(CFLAGS) Ship.cpp

Tanker.o: Tanker.cpp *.h
	$(CC) $(CFLAGS) Tanker.cpp

View.o: View.cpp *.h
	$(CC) $(CFLAGS) View.cpp

Views.o: Views.cpp *.h
	$(CC) $(CFLAGS) Views.cpp	

Warships.o: Warships.cpp *.h
	$(CC) $(CFLAGS) Warships.cpp

Cruise_ship.o: Cruise_ship.cpp *.h
	$(CC) $(CFLAGS) Cruise_ship.cpp

Model.o: Model.cpp *.h
	$(CC) $(CFLAGS) Model.cpp

Ship_factory.o: Ship_factory.cpp *.h
	$(CC) $(CFLAGS) Ship_factory.cpp

Track_base.o: Track_base.cpp *.h
	$(CC) $(CFLAGS) Track_base.cpp

Geometry.o: Geometry.cpp *.h
	$(CC) $(CFLAGS) Geometry.cpp

Navigation.o: Navigation.cpp *.h
	$(CC) $(CFLAGS) Navigation.cpp

Sim_object.o: Sim_object.cpp *.h
	$(CC) $(CFLAGS) Sim_object.cpp

Utility.o: Utility.cpp *.h
	$(CC) $(CFLAGS) Utility.cpp

Group.o: Group.cpp *.h
	$(CC) $(CFLAGS) Group.cpp

Commandable.o: Commandable.cpp *.h
	$(CC) $(CFLAGS) Commandable.cpp

Refuel_ship.o: Refuel_ship.cpp *.h
	$(CC) $(CFLAGS) Refuel_ship.cpp

real_clean:
	rm -f *.o
	rm -f *exe
