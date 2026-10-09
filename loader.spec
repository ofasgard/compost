x64:
	# Load our PIC runner.
	load "bin/loader.x64.o"
	make pic +gofirst +optimize

	# Merge in LibTCG.
	mergelib "lib/libtcg/libtcg.x64.zip"
	
	# Load in our chosen resolver spec (hardcoded for now).
	.resolver.libtcg_ror13
	
	# Load in our chosen retriever spec (hardcoded for now).
	.retriever.linked_raw_dll
	
	# Export as PIC.
	export
	
resolver.libtcg_ror13.x64:
	# Load our DFR resolver and merge it in.
	load "bin/resolvers/libtcg_ror13.x64.o"
	merge
	
	# Add ROR13 dynamic function resolution.
	dfr "resolve" "ror13"
	
retriever.linked_raw_dll.x64:
	# Load our raw DLL retriever and merge it in.
	load "bin/retrievers/linked_raw_dll.x64.o"
	merge

	# Link the payload directly into the implant (no obfuscation).
	push $DLL
	link "linked_capability"
