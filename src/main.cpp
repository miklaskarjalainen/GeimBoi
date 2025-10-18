// SPDX-License-Identifier: GPL-2.0-only
/*
namespace GeimBoi {
extern int main();
}

int main() { return GeimBoi::main(); }
*/

extern int geimboi_main(int argc, char* argv[]);

int main(int argc, char** argv) { return geimboi_main(argc, argv); }
