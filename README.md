# compost

*Mix together all your scraps of unwanted tradecraft, and let's grow something new!*

This is an experiment in collecting composable post-exploitation tradecraft into a single modular implant. The idea is to split out the major functions of a DLL reflective loader, such as decrypting the payload or sleep masking, into separate components. These are mixed in at link-time.

Since the components all follow the same function signature contract, you can freely swap them out just by modifying the `.spec` file. The result is a composable loader where every aspect of its behaviour is modular and extensible.

Currently, we implement the following composable scraps:

- **Transformer**: Defines which Crystal Palace binary transformations are applied to the implant. For example, `optimize_only` corresponds to "make pic +gofirst +optimize".
- **Resolver**: Defines how the implant dynamically resolves Windowss APIs. For example, `libtcg_ror13` uses the ROR13 resolution functions bundled with LibTCG.
- **Retriever**: Defines how the implant retrieves and unpacks the DLL capability. For example, `linked_xor_dll` uses a XOR-obfuscated DLL embedded into the implant with a randomly-generated key.
- **Sleepmask**: Defines how the implant hooks sleep APIs while reflectively loading the capability, and provides an implementation for obfuscating or deobfuscating it in memory. For example, `libtcg_mask` uses an embedded PICO to perform sleepmasking and relies on LibTCG to load it into memory.
- **Loader**: Defines how the implant reflectively loads the DLL capability. For example, `libtcg_reflective` uses the reflective DLL loader implementation bundled with LibTCG.
- **Invoker**: Defines how the implant transfers execution to the loaded DLL capability. For example, `dll_entrypoint` just invokes the entrypoint of the DLL directly.

> **Here Be Dragons!** This project is still in an early stage of development, and the components I've implemented have only been tested against a basic "test" DLL, not a real Beacon or similar DLL capability. This is very much an early WIP, and seeing whether this "composable implant" idea works and is reliable is part of what I'm trying to experiment with. I welcome issues, pull requests, and code contributions. 

## Composing an Implant

To choose which scraps your implant will use, you must modify the `compost.spec` file. For example, to change which transformer the implant uses, look for these lines:

```
# Load in our chosen binary transformer spec (hardcoded for now).
.transformer.optimize_only
```

If you replace `transformer.optimize_only` with `transformer.optimize_mutate`, then the alternate transformer will be used when you build the implant. Scroll to the bottom of `compost.spec` to find the labels that are available for you to use. When this repo is more stable, I'll add some documentation that lists them all in one place.

When you've configured your implant, build the objects and link the capability with Crystal Palace like so:

```sh
$ scripts/build.sh
$ cpl link compost.spec path/to/test.x64.dll compost.bin @config.spec 
```

The resulting `compost.bin` file contains your shellcode.
