CC = g++
LD = g++

CFLAGS = -c -pedantic-errors -std=c++14 -Wall -fno-elide-constructors -g
LFLAGS = -pedantic-errors -Wall

OBJS = p6_main.o Controller.o Island.o Ship.o Tanker.o View.o Grid_view.o Map_view.o Bridge_view.o GPS_view.o Sailing_view.o Cruise_ship.o Warship.o Cruiser.o Torpedo.o Model.o Ship_factory.o Track_base.o Geometry.o Navigation.o Sim_object.o Utility.o Group.o Commandable.o Refuel_ship.o
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

Grid_view.o: Grid_view.cpp *.h
	$(CC) $(CFLAGS) Grid_view.cpp

Map_view.o: Map_view.cpp *.h
	$(CC) $(CFLAGS) Map_view.cpp

Bridge_view.o: Bridge_view.cpp *.h
	$(CC) $(CFLAGS) Bridge_view.cpp

GPS_view.o: GPS_view.cpp *.h
	$(CC) $(CFLAGS) GPS_view.cpp

Sailing_view.o: Sailing_view.cpp *.h
	$(CC) $(CFLAGS) Sailing_view.cpp

Warship.o: Warship.cpp *.h
	$(CC) $(CFLAGS) Warship.cpp

Torpedo.o: Torpedo.cpp *.h
	$(CC) $(CFLAGS) Torpedo.cpp

Cruiser.o: Cruiser.cpp *.h
	$(CC) $(CFLAGS) Cruiser.cpp

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
