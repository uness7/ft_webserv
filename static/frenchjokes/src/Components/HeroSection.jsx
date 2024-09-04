const HeroSection = () => {
	return (
		<div className="bg-primary h-2/3 grid grid-cols-2">
			<div className="mt-16 ml-10 relative">
				<img src="/images/eiffel-tower.png" alt="" className="h-80" />
			</div>
			<div className="absolute top-32 left-60 flex flex-col justify-center items-center">
				<div className="mt-4 ml-8">
					<p className="text-6xl text-center text-white font-pacifico">
						Discover the joy of French humour
					</p>
					<p className="text-xl text-center text-white font-roboto mt-5">
						If you understand the jokes, you understand the
						language.
					</p>
				</div>
				<a
					className="bg-secondary px-20 py-2 mt-10 rounded-lg text-xl text-white"
					href="#jokes"
					rel="noreferrer"
				>
					Have Fun
				</a>
			</div>
		</div>
	)
}

export default HeroSection
