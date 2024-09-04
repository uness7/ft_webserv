const AboutMe = () => {
	return (
		<div className="mx-auto">
			<div className="py-20 flex flex-col justify-center items-center">
				<h1 className="text-4xl font-pacifico mb-10">Who am I</h1>
				<div className="flex flex-row ">
					<div className="w-1/3 flex justify-center items-center">
						<img
							className="h-80 rounded-2xl "
							src="/images/me.jpg"
							alt=""
						/>
					</div>
					<div className="w-2/3 flex justify-center items-center">
						<p className="px-20 py-4 text-2xl text-start mt-10 rounded-2xl mx-20 ">
							Hello, I am Youness Zioual, a passionate full-stack
							web developer with a love for creativity,
							problem-solving, and all things technology. Welcome
							to my personal project, Jokey where you can have fun
							reading French jokes of different types, where I
							combine my programming skills with a touch of humor
							to create a unique online experience.s
						</p>
					</div>
				</div>
			</div>
			<div className="grid grid-cols-3 gap-x-4 mx-2 my-10 text-center">
				<div>
					<h1 className="font-pacifico mb-5">My Journey</h1>
					<p className="text-sm">
						My journey in the world of web development began with a
						curiosity for how things work behind the scenes on the
						internet. Over the years, I have honed my skills in
						various programming languages, frameworks, and
						technologies to bring ideas to life. The art of
						combining beautiful design with seamless functionality
						has always fascinated me.
					</p>
				</div>
				<div>
					<h1 className="font-pacifico  mb-5">Showing My Skills</h1>
					<p className="text-sm">
						Through this project, I wanted to demonstrate my
						proficiency as a full-stack web developer. From crafting
						the user interface and user experience to developing the
						underlying functionality and integrating APIs, every
						aspect of this project showcases my programming
						abilities.
					</p>
				</div>
				<div>
					<h1 className="font-pacifico  mb-5	">
						My Passion for Programming
					</h1>
					<p className="text-sm">
						I am truly passionate about programming and the
						possibilities it offers to create meaningful and
						engaging digital experiences. I am constantly exploring
						new technologies and staying up-to-date with industry
						trends to improve my skills and deliver cutting-edge
						solutions.
					</p>
				</div>
			</div>
			<div className="flex flex-col justify-center items-center">
				<h1 className="text-4xl font-pacifico mb-10">
					How to contact me
				</h1>
				<p className="w-2/3">
					I would love to connect with you and discuss my work
					further. If you have any inquiries, project opportunities,
					or just want to say hello, feel free to reach out to me
					through the contact information provided on the Contact page
					of this website. Thank you for visiting my website. I hope
					you enjoy exploring the French jokes and witnessing the
					culmination of my programming skills and passion for web
					development.
				</p>

				<a
					href="mailto:younes.zioual.dev@gmail.com"
					className="bg-secondary px-20 py-2 rounded-xl my-20 text-white text-xl"
				>
					Contatct Me
				</a>
			</div>
		</div>
	)
}

export default AboutMe
