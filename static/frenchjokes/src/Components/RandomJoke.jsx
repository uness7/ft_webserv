import { useState, useEffect } from 'react'

//const apiKey = import.meta.env.VITE_API_KEY
const apiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyX2lkIjoiODQ0MjU0ODU5MjU5NTQzNTYyIiwibGltaXQiOjEwMCwia2V5IjoieFJRYkxMU3JUQXFNY2FMaHFQSVVSd3ZDUmowTVo4Z014eWJRVEdRVlVFVkphQmFoS1QiLCJjcmVhdGVkX2F0IjoiMjAyNC0wNi0yNVQyMToxMzoxMSswMDowMCIsImlhdCI6MTcxOTM0OTk5MX0.vJqU2r85exI5_evFAsn-oEUQjcl1hWn3fP5btLlHaiI";

const RandomJoke = () => {
	const [data, setData] = useState('')

	useEffect(() => {
		requestJoke()
	}, []) // eslint-disable-line react-hooks/exhaustive-deps

	async function requestJoke() {
		fetch(`https://www.blagues-api.fr/api/random`, {
			headers: {
				Authorization: `Bearer ${apiKey}`,
			},
		})
			.then((res) => res.json())
			.then((data) => {
				setData(data)
			})
	}

	return (
		<div className="h-128 bg-primary grid grid-cols-2 gap-x-6 relative">
			<div className="bg-secondary h-3/5 my-auto rounded-r-3xl flex justify-center items-center">
				<p className="text-2xl text-white text-center">
					{data.joke}
					{data.answer}
				</p>
			</div>
			<div className="my-auto text-white font-roboto text-xl">
				<p className="px-6 py-2">
					Whether you are a language enthusiast, a fan of French
					culture, or simply seeking a good laugh, our website offers
					a delightful collection of French jokes. Check out this
					radndom one!
				</p>
				<div className="flex justify-center items-center mt-10">
					<button
						onClick={(e) => {
							e.preventDefault()
							requestJoke()
						}}
						className="px-16 py-2 text-center rounded-xl bg-secondary text-xl"
					>
						More
					</button>
				</div>
			</div>
		</div>
	)
}

export default RandomJoke
