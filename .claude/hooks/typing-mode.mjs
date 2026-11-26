// 타이핑 모드: src/ · tests/ 아래 C++ 파일을 Claude Code가 새로 쓰거나 통째로 덮어쓰지 못하게 막는다.
// 기존 파일의 부분 수정(Edit 도구)은 막지 않는다 — CLAUDE.md 규칙대로 diff를 보여 주고 승인 후 수정.
// PDF 19장 원본에서 바뀐 점: PowerShell 도구도 검사 (2026-11-26 사용자 결정, questions.md Q1).
// 2026-11-26 저장소 구조 결정: C++ 앱은 BE/src·BE/tests. 경로에 src/·tests/가 그대로 들어가므로 검사식은 바꾸지 않았다.
let raw = '';
process.stdin.on('data', (chunk) => (raw += chunk));
process.stdin.on('end', () => {
  let input = {};
  try { input = JSON.parse(raw || '{}'); } catch { process.exit(0); }

  const tool = input.tool_name;
  const args = input.tool_input || {};
  const norm = (p) => String(p || '').replace(/\\/g, '/');
  const CPP = /\.(c|cc|cpp|cxx|h|hh|hpp|hxx|ixx|cppm|inl)$/i;
  // 2026-11-26 사용자 결정: 테스트 코드(tests/)는 Claude Code가 쓴다. src/만 타이핑 대상.
  const isTypedCpp = (p) => /(^|\/)src\//.test(norm(p)) && CPP.test(norm(p));

  const block = (reason) => {
    process.stderr.write(
      `[타이핑 모드] ${reason}\n` +
      '파일 경로와 전체 코드를 채팅에 보여 주고, 사용자가 "작성 완료"라고 할 때까지 기다리세요.\n'
    );
    process.exit(2);
  };

  if (tool === 'Write' && isTypedCpp(args.file_path)) {
    block(`${norm(args.file_path)} 는 사용자가 직접 타이핑하는 C++ 파일입니다.`);
  }

  if (tool === 'Bash' || tool === 'PowerShell') {
    const cmd = String(args.command || '');
    const touchesCpp = /(src|tests)[\\/]\S*\.(c|cc|cpp|cxx|h|hh|hpp|hxx|ixx|cppm|inl)\b/i.test(cmd);
    const writes = /(>|\btee\b|\bcp\b|\bcopy\b|\bmv\b|\bmove\b|\btouch\b|\bsed\s+-i|Set-Content|Out-File|New-Item)/i.test(cmd);
    if (touchesCpp && writes) {
      block('셸 명령으로 src·tests의 C++ 파일을 만들거나 바꿀 수 없습니다.');
    }
  }

  process.exit(0);
});
