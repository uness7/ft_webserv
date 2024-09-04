/* eslint-disable jsx-a11y/anchor-is-valid */
import { AiFillGithub } from 'react-icons/all'
import { Link } from 'react-router-dom'

const Footer = () => {
	return (
		<div className="h-48 px-8 py-4 text-center bg-primary text-white flex flex-col justify-evenly ">
			<div className="grid grid-cols-3">
				<div className="text-center flex flex-col justify-center items-center">
					<h1 className="mb-3 text-3xl font-fredoka cursor-pointer">
						Youness Zioual
					</h1>
					<p className="text-sm">
						A passionate web developer. Say Hi !
					</p>
					<div className="flex flex-row space-x-1">
						<a
							href="https://www.github.com/uness7"
							className="cursor-pointer mt-4 text-4xl"
						>
							<AiFillGithub></AiFillGithub>
						</a>
					</div>
				</div>
				<div className="text-center">
					<p>
						Big shoutout to
						<a href="https://www.github.com/zuruuh">
							<span className="cursor-pointer"> @Zuruuh </span>
						</a>
						who made this great API free of charge. Check it out at
						-
						<a
							className="cursor-pointer"
							href="https://www.blagues-api.fr"
						>
							<span className="underline text-blue-700">
								blagues-francaises.com
							</span>
						</a>
					</p>
				</div>
				<div className="flex flex-col underline cursor-pointe text-center">
					<Link to="/">Home</Link>
					<Link to="/about-me">About me</Link>
					<a href="https://www.paypal.me/waiziiii">PayPal me</a>
				</div>
			</div>
			<div className="text-lg">
				<p>Created In Casablanca with ❤️</p>
			</div>
		</div>
	)
}

export default Footer
