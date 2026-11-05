/* Rebuild the boot image from the CURRENT T:/ tree, headless.
 *
 * This is the call the GUI makes, spelled the way a human types it:
 *   Cd("T:/");  Cmp("FULL_PACKAGE.HC", "HCRT.NB.DBG.Z", "HCRT.NB.BIN");
 * Two rules learned the hard way on this drive, both recorded in
 * T:/CC/GATES/README.txt gates 7 and 8:
 *   1. A -c run starts in the HOST cwd mounted at Z: (main.c:167-176), so a
 *      RELATIVE name does not mean T:/.  Without the Cd() the compile looked
 *      for <host-cwd>/FULL_PACKAGE.HC, "succeeded" at once and wrote a 48-byte
 *      EMPTY MODULE over the destination - after which every boot died with a
 *      bare SIGSEGV and no message, because tos_aot.c:250 dereferences an
 *      unchecked symbol lookup before SetupDebugger().
 *   2. Output goes to a NEW name, never straight onto ./HCRT.BIN, and the old
 *      pair is kept OUTSIDE the tree until the new one is gated.  A failed Cmp
 *      overwrites its out_name.
 *
 * Expect: `Errs:0 ... Code:<n> Size:<hex>` and `RB done` (the compiler's own
 * summary line prints before it).  Then, from the repo root:
 *   cp T/HCRT.NB.BIN HCRT.BIN && cp T/HCRT.NB.BIN T/HCRT.BIN
 *   cp T/HCRT.NB.DBG.Z T/HCRT.DBG.Z
 * because the boot reads T:/HCRT.DBG.Z while ./HCRT.BIN is resolved from the
 * process cwd (FULL_PACKAGE.HC's Adam: `DbgInfoRead("HCRT.DBG.Z")`), so the
 * image and its map are a pair and must move together.
 *
 * Run:  ./3d_loader -t T -c T:/CC/GATES/rebuild.HC
 */
Cd("T:/");
Cmp("FULL_PACKAGE.HC", "HCRT.NB.DBG.Z", "HCRT.NB.BIN");
"RB done\n";
Exit;
