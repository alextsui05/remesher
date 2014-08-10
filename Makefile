all:
	$(MAKE) -C libremesh
	$(MAKE) -C libglstuff
	$(MAKE) -C gtkremesher


multicore:
	$(MAKE) -C libremesh multicore
	$(MAKE) -C libglstuff multicore
	$(MAKE) -C gtkremesher multicore

clean:
	$(MAKE) -C libremesh clean
	$(MAKE) -C libglstuff clean
	$(MAKE) -C gtkremesher clean
	$(MAKE) -C cmdremesher clean

depend:
	$(MAKE) -C libremesh depend
	$(MAKE) -C libglstuff depend
	$(MAKE) -C gtkremesher depend
	$(MAKE) -C cmdremesher depend
