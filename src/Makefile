GCC ?= gcc
NVCC ?= nvcc
BACKEND ?= x86
CFLAGS ?= -std=gnu99 -Wall -fopenmp -g
LDLIBS ?= -lm
CUDA_ARCH ?=

COMMON_C_SRCS := main.c parser.c levelization.c netlist.c simulation.c flat_netlist.c backend_legacy.c backend_flat_cpu.c
CPU_SRCS := $(COMMON_C_SRCS) backend_cuda_stub.c
CUDA_C_SRCS := $(COMMON_C_SRCS)
CUDA_CU_SRCS := backend_cuda.cu backend_cuda_kernel.cu
CUDA_OBJS := $(CUDA_C_SRCS:.c=.o) $(CUDA_CU_SRCS:.cu=.o)

ifeq ($(BACKEND),cuda)
    ifeq (, $(shell command -v $(NVCC) 2>/dev/null))
        $(error BACKEND=cuda requested, but '$(NVCC)' was not found. Use BACKEND=x86 or BACKEND=flat on CPU-only systems)
    endif
endif

.PHONY: all sanitize fast clean

all:
ifeq ($(BACKEND),cuda)
	$(MAKE) cuda-build
else ifeq ($(BACKEND),x86)
	$(GCC) $(CFLAGS) $(CPU_SRCS) -o main $(LDLIBS)
else ifeq ($(BACKEND),legacy)
	$(GCC) $(CFLAGS) $(CPU_SRCS) -o main $(LDLIBS)
else ifeq ($(BACKEND),flat)
	$(GCC) $(CFLAGS) $(CPU_SRCS) -o main $(LDLIBS)
else
	$(error Unsupported BACKEND='$(BACKEND)'. Use x86, legacy, flat, or cuda)
endif

sanitize:
ifeq ($(BACKEND),cuda)
	$(MAKE) cuda-build
else
	$(GCC) -fsanitize=address $(CFLAGS) $(CPU_SRCS) -o main $(LDLIBS)
endif

fast:
ifeq ($(BACKEND),cuda)
	$(MAKE) cuda-build CFLAGS="-std=gnu99 -O3 -Wall -fopenmp -g"
else
	$(GCC) -Ofast $(CFLAGS) $(CPU_SRCS) -o main $(LDLIBS)
endif

cuda-build: $(CUDA_OBJS)
	$(NVCC) $(CUDA_ARCH) $(CUDA_OBJS) -o main $(LDLIBS)

%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@

%.o: %.cu
	$(NVCC) $(CUDA_ARCH) -O3 -x cu -c $< -o $@

clean:
	rm -f main *.o
