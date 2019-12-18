
# Memory management

It contains 2 modules and an application that test how much memory can get
request using:

- get_free_pages()
- kmalloc()
- vmalloc()

```shell
make
sudo insmod lab_maxmem.ko
sudo insmod lab_maxvmalloc.ko
sudo rmmod lab_maxmem
sudo rmmod lab_maxvmalloc
```

