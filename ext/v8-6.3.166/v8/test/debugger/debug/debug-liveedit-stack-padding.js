// Copyright 2012 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Debug = debug.Debug;
Debug.setListener(listener);

SlimFunction = eval(
    "(function() {\n " +
    "  return 'Cat';\n" +
    "})\n"
);

var script = Debug.findScript(SlimFunction);

Debug.setScriptBreakPointById(script.id, 1, 0);

var orig_animal = "'Cat'";
var patch_pos = script.source.indexOf(orig_animal);
var new_animal_patch = "'Capybara'";

debugger_handler = (function() {
  var already_called = false;
  return function() {
    if (already_called) {
      return;
    }
    already_called = true;

    var change_log = new Array();
    try {
      Debug.LiveEdit.TestApi.ApplySingleChunkPatch(script, patch_pos,
          orig_animal.length, new_animal_patch, change_log);
    } finally {
      print("Change log: " + JSON.stringify(change_log) + "\n");
    }
  };
})();

var saved_exception = null;

function listener(event, exec_state, event_data, data) {
  if (event == Debug.DebugEvent.Break) {
    try {
      debugger_handler();
    } catch (e) {
      saved_exception = e;
    }
  } else {
    print("Other: " + event);
  }
}


var animal = SlimFunction();

if (saved_exception) {
  print("Exception: " + saved_exception);
  assertUnreachable();
}

assertEquals("Capybara", animal);

Debug.setListener(null);
