# compost

*Mix together all your scraps of unwanted tradecraft, and let's grow something new!*

This is an experiment in collecting composable post-exploitation tradecraft into a single modular implant. The idea is to split out the major functions of a DLL reflective loader, such as decrypting the payload or sleep masking, into separate components. These are mixed in at link-time.

Since the components all follow the same function signature contract, you can freely swap them out just by modifying the `config.spec` file. The result is a composable loader where every aspect of its behaviour is modular and extensible.

Currently, we implement the following composable "scraps" of tradecraft:

- **Transformer**: Defines which Crystal Palace binary transformations are applied to the implant. For example, `optimize_only` corresponds to "make pic +gofirst +optimize".
- **Resolver**: Defines how the implant dynamically resolves Windowss APIs. For example, `libtcg_ror13` uses the ROR13 resolution functions bundled with LibTCG.
- **Retriever**: Defines how the implant retrieves and unpacks the DLL capability. For example, `linked_xor_dll` uses a XOR-obfuscated DLL embedded into the implant with a randomly-generated key.
- **Sleepmask**: Defines how the implant hooks sleep APIs while reflectively loading the capability, and provides an implementation for obfuscating or deobfuscating it in memory. For example, `libtcg_gpa` uses an embedded PICO to perform sleepmasking and relies on LibTCG to load it into memory.
- **Loader**: Defines how the implant reflectively loads the DLL capability. For example, `libtcg_reflective` uses the reflective DLL loader implementation bundled with LibTCG.
- **Invoker**: Defines how the implant transfers execution to the loaded DLL capability. For example, `dll_entrypoint` just invokes the entrypoint of the DLL directly.

> **Here Be Dragons!** This project is still in an early stage of development. It's very much an early WIP, and seeing whether this "composable implant" idea works and is reliable is part of what I'm trying to experiment with. I welcome issues, pull requests, and ideas about what kind of configurable/composable functionality this project should include. 

## Composing an Implant

To choose which scraps your implant will use, you must modify the `config.spec` file. For example, to change which transformer the implant uses, look for this line:

```
setg "%COMPOST_TRANSFORMER" "transformer.optimize_only"
```

If you replace `transformer.optimize_only` with `transformer.optimize_mutate`, then the alternate transformer will be used when you build the implant. To find the labels that are available for you to use, check `scraps.spec`. When this repo is more stable, I'll add some documentation that lists them all in one place.

When you've configured your implant, build the objects and link the capability with Crystal Palace like so:

```sh
$ scripts/build.sh
$ cpl link compost.spec path/to/test.x64.dll compost.bin @config.spec 
```

The resulting `compost.bin` file contains your shellcode.

## Expanding and Contributing Tradecraft

If you wanted to add (for example) a new reflective loader to the project, here's how you'd do it:

1. Write your loader source code in `src/loaders/`, using the existing modules as a template. 
2. Modify the `Makefile` to ensure your source code gets compiled into object files by the build script. 
3. Add a new label to `scraps.spec` such as `loader.my_cool_loader.x64` which defines how Crystal Palace should load and merge/link your loader.

The most important thing is that your code follows the APIs defined in `compost.h`; this is how we ensure compatibility between the modular components. If you're writing a reflective loader, for example, you should have a `load_dll()` function that takes a pointer to a raw DLL and populates the `LOADED_DLL` struct provided. If you want your loader to support sleepmasking via hooking `GetProcAddress()`, it should use `get_sleep_hook()` to retrieve the `GetProcAddress` pointer that your loader will pass to the capability.

## Disclaimer

*This project is released strictly for educational purposes, and is intended solely for use by authorised parties performing legitimate security research and red team assessments. My only intention is to share my work for the purposes of uplifting security.*
