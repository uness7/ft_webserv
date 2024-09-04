import { Link } from 'react-router-dom'

const Navbar = () => {
	return (
		<div className="h-16 m-auto bg-primary text-white font-roboto text-xs">
			<div className="w-full pt-8 flex justify-center items-center text-center gap-x-64 cursor-pointer text-base">
				<Link to="/">Home</Link>
				<Link to="/about-me">About me</Link>
			</div>
		</div>
	)
}

export default Navbar
