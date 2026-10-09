x64:
	meta "name" "compost"
	meta "describe" "A composable reflective loader."
	meta "license" "GNU GPL v2"
	meta "author" "Callum Murphy-Hale @cactzone"

	# Load our central module.
	load "bin/compost.x64.o"
	make pic +gofirst +optimize

	# Merge in LibTCG.
	mergelib "lib/libtcg/libtcg.x64.zip"
	
	# Load in our chosen resolver spec (hardcoded for now).
	.resolver.libtcg_ror13
	
	# Load in our chosen retriever spec (hardcoded for now).
	.retriever.linked_raw_dll
	
	# Load in our chose loader spec (hardcoded for now).
	.loader.libtcg_reflective
	
	# Load in our chosen invoker spec (hardcoded for now).
	.invoker.dll_entrypoint
	
	# Export as PIC.
	export

# Resolver Labels

resolver.libtcg_ror13.x64:
	# Load our DFR resolver and merge it in.
	load "bin/resolvers/libtcg_ror13.x64.o"
	merge
	
	# Add ROR13 dynamic function resolution.
	dfr "resolve" "ror13"
	
# Retriever Labels
	
retriever.linked_raw_dll.x64:
	# Load our raw DLL retriever and merge it in.
	load "bin/retrievers/linked_raw_dll.x64.o"
	merge

	# Link the payload directly into the implant (no obfuscation).
	push $DLL
	link "linked_capability"
	
retriever.linked_xor_dll.x64:
	# Load our XOR DLL retriever and merge it in.
	load "bin/retrievers/linked_xor_dll.x64.o"
	merge
	
	# Generate a 32-byte encryption key and patch it in.
	generate $XOR_KEY 32
	patch "XOR_KEY" $XOR_KEY
	
	# Encrypt and link the payload (with prepended length so you know how much to decrypt).
	push $DLL
	mask "xor" $XOR_KEY
	preplen
	link "linked_capability"

# Loader Labels

loader.libtcg_reflective.x64:
	# Load our basic LibTCG reflective loader and merge it in.
	load "bin/loaders/libtcg_reflective.x64.o"
	merge

# Invoker Labels

invoker.dll_entrypoint.x64:
	# Load our basic DLL entrypoint invoker and merge it in.
	load "bin/invokers/dll_entrypoint.x64.o"
	merge
