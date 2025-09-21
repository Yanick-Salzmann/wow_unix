# GitHub Copilot Instructions

This repository uses a repo-level instructions file to guide Copilot behavior in IDEs (JetBrains/IntelliJ, VS Code, Neovim).

## Hard rules
- Do not generate comments in code under any circumstances.
- Do not add docstrings or documentation comments (JSDoc/TSDoc/KDoc/JavaDoc), line comments, or block comments.
- Do not insert commented-out code or license headers.
- When completing, return only executable code; explanations belong in chat, not in code.
- Do not generate tests or test files.
- Do not create or modify README files or other documentation files.
- Do not use ngIf and ngFor directives in angular. Use @if and @for instead.

## Style and scope
- Follow existing project structure and patterns without reformatting unrelated code.
- Keep diffs minimal: only touch what’s needed for the task.
- Preserve import style and public APIs unless explicitly instructed to change them.

## Language notes
- TypeScript/Angular: Prefer Angular standalone components and current Angular idioms observed in the repo; avoid adding comments or JSDoc.
- C++/CMake: Keep implementations concise and comment-free; avoid adding banner or inline comments.

## Testing
- Do not generate tests or test scaffolding.

If a prompt asks for commented code, prefer producing clear, self-explanatory code without comments and provide any explanations in chat instead of inline comments.
