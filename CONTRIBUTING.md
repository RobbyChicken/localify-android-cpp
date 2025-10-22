# Contributing to Localify Android C++

Thank you for your interest in contributing to the Localify Android C++ app! This document provides guidelines for contributing to this pure C++ Android implementation.

## 🚀 Getting Started

### Prerequisites
- Android NDK r25 or later
- CMake 3.22.1 or later
- Android SDK API 24+ (minimum) / API 34 (target)
- Git

### Development Setup
1. Clone the repository
2. Open in Android Studio or your preferred C++ IDE
3. Ensure NDK and CMake paths are configured
4. Build with `./gradlew assembleDebug`

## 📋 Code Style Guidelines

### C++ Standards
- Use C++17 standard
- Follow Google C++ Style Guide
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and small

### File Organization
- Headers in `app/src/main/cpp/include/`
- Sources in `app/src/main/cpp/src/`
- Follow existing naming conventions
- Include guards for all headers

### Memory Management
- Use RAII principles
- Prefer smart pointers when appropriate
- Manual memory management for performance-critical code
- Always check for null pointers

## 🏗️ Architecture Guidelines

### UI Components
- Extend base `UIComponent` class
- Implement proper touch handling
- Use OpenGL ES 2.0 for rendering
- Maintain consistent styling with `AppConfig`

### API Integration
- Use `HttpClient` for all network requests
- Handle errors gracefully
- Implement proper authentication
- Follow existing API patterns

### Screen Implementation
- Extend `Screen` base class
- Implement proper lifecycle methods
- Handle navigation consistently
- Use bottom navigation pattern

## 🧪 Testing

### Manual Testing
- Test on multiple Android versions (API 24+)
- Verify on different screen sizes
- Test touch interactions
- Validate API responses

### Performance Testing
- Monitor memory usage
- Check rendering performance
- Validate network efficiency
- Test on low-end devices

## 📝 Pull Request Process

1. **Fork the repository**
2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. **Make your changes**
   - Follow code style guidelines
   - Add appropriate comments
   - Test thoroughly
4. **Commit your changes**
   ```bash
   git commit -m "feat: add your feature description"
   ```
5. **Push to your fork**
   ```bash
   git push origin feature/your-feature-name
   ```
6. **Create a Pull Request**
   - Provide clear description
   - Reference any related issues
   - Include testing information

### Commit Message Format
Use conventional commits:
- `feat:` for new features
- `fix:` for bug fixes
- `docs:` for documentation
- `style:` for formatting changes
- `refactor:` for code refactoring
- `test:` for adding tests
- `chore:` for maintenance tasks

## 🐛 Bug Reports

When reporting bugs, please include:
- Android version and device model
- Steps to reproduce
- Expected vs actual behavior
- Relevant logs or screenshots
- Code snippets if applicable

## 💡 Feature Requests

For new features:
- Describe the use case
- Explain the expected behavior
- Consider implementation complexity
- Discuss potential alternatives

## 🔧 Development Areas

### High Priority
- Performance optimizations
- Memory leak fixes
- UI/UX improvements
- API error handling

### Medium Priority
- Additional screen features
- Enhanced map functionality
- Better touch interactions
- Code documentation

### Low Priority
- Code refactoring
- Build system improvements
- Development tooling
- Additional platforms

## 📚 Resources

- [Android NDK Documentation](https://developer.android.com/ndk)
- [OpenGL ES 2.0 Reference](https://www.khronos.org/opengles/sdk/docs/man/)
- [CMake Documentation](https://cmake.org/documentation/)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

## 🤝 Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Help others learn and grow
- Focus on the code, not the person
- Maintain professional communication

## 📞 Getting Help

- Open an issue for bugs or questions
- Check existing issues before creating new ones
- Provide detailed information
- Be patient and respectful

Thank you for contributing to Localify Android C++! 🎵
