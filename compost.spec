x64:
	meta "name" "compost"
	meta "describe" "A composable reflective loader."
	meta "license" "GNU GPL v2"
	meta "author" "Callum Murphy-Hale @ofasgard"

	echo "Let's make some compost!"

	# Load our central module.
	load "bin/compost.x64.o"

	# Load in our chosen binary transformer spec (hardcoded for now).
	call "scraps.spec" %COMPOST_TRANSFORMER

	# Merge in LibTCG.
	mergelib "lib/libtcg/libtcg.x64.zip"
	
	# Load in our chosen resolver spec (hardcoded for now).
	call "scraps.spec" %COMPOST_RESOLVER
	
	# Load in our chosen retriever spec (hardcoded for now).
	call "scraps.spec" %COMPOST_RETRIEVER
	
	# Load in our chosen sleepmask spec (hardcoded for now).
	call "scraps.spec" %COMPOST_SLEEPMASK
	
	# Load in our chosen loader spec (hardcoded for now).
	call "scraps.spec" %COMPOST_LOADER
	
	# Load in our chosen invoker spec (hardcoded for now).
	call "scraps.spec" %COMPOST_INVOKER
	
	# Export as PIC.
	export
