# compost

*Mix together all your scraps of unwanted tradecraft, and let's grow something new!*

This is a (currently very early) framework for composable post-exploitation tradecraft. The idea is to split out the major functions of a DLL capability loader, such as decrypting the payload or resolving WIN32 APIs, into separate components. These are mixed in at link-time.

Since the components all follow the same function signature contract, you can freely swap them out just by modifying the `.spec` file. The result is a composable loader where every aspect of its behaviour is modular and extensible.

I'm also planning to create a companion tool that works a bit like a wizard, walking you through the process and asking you which bits of tradecraft you wish to include at every step. This should make it easy to quickly iterate payloads while you're performing your offensive detection engineering.

Stuff to do:

- Some kind of sleep masking implementation
- Other APIs?
- Some nice documentation, including how to contribute tradecraft scraps

***

Test with:

```sh
$ cpl link compost.spec demo/test.x64.dll compost.bin @config.spec 
```
