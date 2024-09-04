const Intro = () => {
	return (
		<div
			id="jokes"
			className="grid grid-cols-2 justify-center items-center my-20"
		>
			<div className="text-2xl text-center w-96 m-auto">
				<p className="font-pacifico">
					Welcome to Jokey, where laughter meets the charm of French
					humor! Discover a world of joy and amusement as we bring you
					an assortment of hilarious jokes straight from the heart of
					France.
				</p>
			</div>
			<div className="m-auto">
				<img
					src="/images/frenchjokes-img2.jpg"
					className="h-80"
					alt=""
				/>
			</div>
		</div>
	)
}

export default Intro
