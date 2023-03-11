# LoriBirdOS Kernel #
**Kernel modules** are located in [modules](/modules)

`modules.cfg` is copied to the initrd. At boot the kernel reads a list of filepaths from `modules.cfg` and loads the respective modules.
