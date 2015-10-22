/*
 * Copyright (C) 2015 Unikernel Systems
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <asl.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/threads.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define Asl_val(v) (*((aslclient *) Data_custom_val(v)))

static void client_finalize(value v) {
  asl_close(Asl_val(v));
}

static struct custom_operations client_ops = {
  "org.mirage.caml.asl.client",
  client_finalize,
  custom_compare_default,
  custom_hash_default,
  custom_serialize_default,
  custom_deserialize_default
};

static value alloc_client(aslclient asl) {
  value v = alloc_custom(&client_ops, sizeof(aslclient), 0, 1);
  Asl_val(v) = asl;
  return v;
}

CAMLprim value stub_asl_open(value ident, value facility, value stderr, value no_delay, value no_remote) {
  CAMLparam5(ident, facility, stderr, no_delay, no_remote);
  const char *c_ident = String_val(ident);
  const char *c_facility = String_val(facility);
  uint32_t options =
      (Bool_val(stderr)?ASL_OPT_STDERR:0)
    | (Bool_val(no_delay)?ASL_OPT_NO_DELAY:0)
    | (Bool_val(no_remote)?ASL_OPT_NO_REMOTE:0);
  aslclient asl = NULL;

  caml_release_runtime_system();
  asl = asl_open(c_ident, c_facility, options);
  caml_acquire_runtime_system();

  CAMLreturn(alloc_client(asl));
}

