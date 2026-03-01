# AQA A-Level Computer Science NEA Presentation
## Scientific Calculator Project

---

## Slide 1: Title Slide
**What is an NEA? My A-Level Computer Science Project**

_Presenter: [Your Name]_
_AQA A-Level Computer Science_

**Notes:**
Hello everyone! Today I'm going to talk about my A-Level Computer Science project. I know most of you probably haven't done Computer Science yet, so I'll start by explaining what an NEA actually is. NEA stands for Non-Exam Assessment - basically, it's a big coursework project that makes up 20% of your final A-Level grade. Instead of just sitting exams, you get to spend months building a real piece of software that solves an actual problem. Think of it like your coursework in other subjects, but instead of writing essays, you're creating working computer programs.

---

## Slide 2: What is an NEA?
**The Non-Exam Assessment Explained**

• **20% of your final A-Level grade**
• **A real programming project** - not just theory!
• **4 main stages:**
  - Analysis: What problem are you solving?
  - Design: How will you build it?
  - Development: Actually coding it
  - Evaluation: Did it work?

**Notes:**
So what exactly is the NEA? In AQA Computer Science, the NEA is worth 20% of your final grade, which is pretty significant. Unlike traditional exams where you answer questions, the NEA requires you to actually build a real piece of software from scratch. You go through four main stages: First, Analysis - where you identify a problem that needs solving and figure out what your program needs to do. Second, Design - where you plan out exactly how you'll build it, what features it needs, and how users will interact with it. Third, Development - this is the actual coding part where you spend months writing the program. And finally, Evaluation - where you test everything, fix bugs, and see if it actually solved the problem you set out to fix. The whole process takes around 6-9 months from start to finish.

---

## Slide 3: My Project - Scientific Calculator
**Building a Calculator for Engineers & Scientists**

• **Problem:** Students need a scientific calculator for A-Level Maths & Physics
• **Solution:** A custom calculator that:
  - Works on physical hardware (not just a computer screen)
  - Has advanced features like graphing and equation solving
  - Looks and feels like a real Casio calculator

**Notes:**
For my NEA, I decided to build a scientific calculator. You might be thinking "don't calculators already exist?" - and you'd be right! But here's the thing: I wanted to create something that works on actual physical hardware that you can hold in your hand, not just software on a computer. The problem I identified was that students doing A-Level Maths and Physics need calculators with advanced features - things like graphing functions, solving equations, and doing statistical calculations. My goal was to create a calculator that looks and feels like the Casio calculators you use in class, but is completely custom-built from scratch. This means I had to program everything - from how buttons work, to how numbers are displayed on screen, to the actual mathematics behind scientific calculations.

---

## Slide 4: The Technology Behind It
**How Does It Actually Work?**

• **Hardware:** ESP32 microcontroller (a tiny computer chip)
• **Display:** LCD screen (like your phone, but simpler)
• **Buttons:** Physical keypad (like a real calculator)
• **Programming Language:** C programming
• **Lines of Code:** Over 8,000 lines!

**Notes:**
Let me explain the technology that makes this work - don't worry, I'll keep it simple! The "brain" of the calculator is called an ESP32 - it's a tiny computer chip about the size of a coin that you can program to do whatever you want. It has a small LCD screen to display numbers and graphs, and a physical button keypad that you press just like a normal calculator. I programmed it using a language called C, which is one of the fundamental programming languages used in real-world devices like cars, phones, and appliances. By the time I finished, I'd written over 8,000 lines of code - that's like writing a 100-page essay, except every single word has to be perfect or the whole thing breaks! Each line tells the computer exactly what to do, whether that's "when button 5 is pressed, display a 5" or "calculate the sine of this angle."

---

## Slide 5: Cool Features I Built
**What Can It Actually Do?**

• **Math Mode:** Basic and advanced calculations (sin, cos, powers, roots)
• **Graph Mode:** Plot multiple functions on screen
• **Solver Mode:** Solve equations automatically
• **Statistics Mode:** Calculate probabilities and distributions
• **Mechanics Mode:** Solve physics motion problems (SUVAT)

**Notes:**
So what can this calculator actually do? Well, it has five main apps or modes. Math Mode is your basic calculator - you can do addition, multiplication, but also advanced stuff like trigonometry, powers, and square roots. Graph Mode lets you type in mathematical functions and it draws the graphs on screen - you can zoom in, zoom out, and even trace along the curve to see exact values. Solver Mode is really cool - you can type in an equation and it will solve it for you automatically using something called Newton-Raphson method, which is an algorithm for finding solutions. Statistics Mode helps with probability - you can calculate things like normal distributions and binomial probabilities, which are crucial for A-Level Maths. And finally, Mechanics Mode solves physics SUVAT equations for you - that's the motion equations you learn in A-Level Physics with displacement, velocity, acceleration, and time. The calculator figures out the unknown values based on what you give it.

---

## Slide 6: Challenges & Problem Solving
**The Hard Parts**

**Challenges I faced:**
• Making buttons work reliably (debouncing)
• Creating custom fonts for mathematical symbols (π, √, ²)
• Building a reusable menu system across different apps
• Fixing the graph editor keyboard input bugs

**Solution approach:**
• Test, debug, fix - repeat hundreds of times!
• Research how real calculators handle these problems
• Break big problems into smaller pieces

**Notes:**
Building this wasn't easy - I ran into loads of challenges along the way. One of the first problems was making the buttons work reliably. When you press a physical button, it actually "bounces" and sends multiple signals - I had to write code to filter that out, which is called debouncing. Another big challenge was creating custom fonts to display mathematical symbols like pi, square roots, and superscript numbers - computers don't just "know" how to show these, you have to design every single pixel. Later in the project, I had to build a reusable menu system so that all the different calculator modes had consistent navigation - this meant planning out a design pattern that could work across different apps. One of the trickiest bugs to fix was in the graph editor - when you pressed the 'x' key, it would sometimes type 'V' instead! This took days to debug, but I eventually figured out it was because keyboard events were being processed twice. The key to solving all these problems was breaking them down into smaller pieces, testing constantly, and not giving up when things went wrong.

---

## Slide 7: Key Takeaways & Conclusion
**What I Learned**

**Skills developed:**
• Programming in C (low-level hardware control)
• Problem solving & debugging
• Project management over 6+ months
• User interface design

**Why this matters:**
• Real-world software development experience
• Understanding how everyday devices actually work
• Preparing for university Computer Science courses

**Questions?**

**Notes:**
So what did I actually learn from doing this NEA? Well, obviously I got much better at programming in C, which is a really powerful language used in embedded systems - that's devices like calculators, cars, medical equipment, and smart appliances. But beyond just coding, I learned how to debug complex problems, which means figuring out why something isn't working and fixing it. I also learned project management - keeping track of what needs to be done, prioritizing features, and managing my time over six months. And I learned about user interface design - making sure the calculator is actually pleasant and intuitive to use, not just functional. Why does this matter? Well, this NEA gives you real-world software development experience that universities and employers actually value. You're not just learning theory from textbooks - you're building something real that actually works. It helps you understand how the devices you use every day actually function under the hood. And if you're thinking about studying Computer Science at university, the NEA is excellent preparation because it teaches you the problem-solving approach that professional developers use every single day. Thanks for listening - does anyone have any questions?

